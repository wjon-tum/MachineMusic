#include <chrono>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <thread>

#include "MetaEvent.h"
#include "MidiEvent.h"
#include "Track.h"

std::vector<char> readFileToBuffer(const std::string &);

std::vector<std::string> convertTrackToneData(Track &);

int verifyMidiHeader(const std::vector<unsigned char> &, Track &);

int computeNumberFromBytes(std::initializer_list<unsigned char>);

int computeNumberFromBytes(std::vector<int>);

int computeTimeFromMidi(long, int, int);

bool sendSerialData(const std::vector<std::string> &, const char*, int);

double computeFrequencyFromMidi(int);

void readMidiEvents(int, int, const std::vector<unsigned char> &, Track &);

void computeTonesFromEvents(Track &);

void failFileFormat();

std::vector<char> HEADER_DATA = {0x4d, 0x54, 0x68, 0x64, 0x00, 0x00, 0x00, 0x06};
std::vector<char> CHUNK_DATA = {0x4d, 0x54, 0x72, 0x6b};
constexpr int A4 = 443;
auto PORT = "/dev/ttyACM0";
constexpr int BAUD = 115200;


int main(const int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <Midi-File>" << std::endl;
        return EXIT_FAILURE;
    }

    auto track = Track();
    const std::vector<char> tmp = readFileToBuffer(argv[1]);
    const std::vector<unsigned char> data(tmp.begin(), tmp.end());
    const int length = verifyMidiHeader(data, track);
    readMidiEvents(22, length, data, track);
    computeTonesFromEvents(track);
    std::vector<std::string> toneData = convertTrackToneData(track);
    for (auto &tone: toneData) {
        std::cout << tone << std::endl;
    }

    if (sendSerialData(toneData, PORT, BAUD) < 1) {
        std::cerr << "Failed to send data to serial port" << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}

int verifyMidiHeader(const std::vector<unsigned char> &data, Track &track) {
    if (data.size() < HEADER_DATA.size()) {
        std::cerr << "Midi header too small" << std::endl;
        failFileFormat();
    }

    for (int i = 0; i < HEADER_DATA.size(); i++) {
        if (data[i] != HEADER_DATA[i]) {
            failFileFormat();
        }
    }

    if (data[8] != 0x00 || (data[9] != 0x00 && (data[9] != 0x01 || data[11] != 0x01))) {
        //todo accept multiple tracks
        std::cerr << "Midi header doesn't match" << std::endl;
        failFileFormat();
    }

    track.ticksPerQuarter = computeNumberFromBytes({data[12], data[13]});

    for (int i = 14; i < 18; i++) {
        if (data[i] != CHUNK_DATA[i - 14]) {
            std::cerr << "Midi chunk doesn't match" << std::endl;
            failFileFormat();
        }
    }

    return computeNumberFromBytes({data[18], data[19], data[20], data[21]});
}

void readMidiEvents(int from, int length, const std::vector<unsigned char> &data, Track &track) {
    if (from + length > data.size()) {
        failFileFormat();
    }

    int runningStatus = 0;
    while (length > 0) {
        long deltaTime = 0;
        while (data[from] & 0x80) {
            deltaTime += data[from++] & 0x7F;
            deltaTime *= 128;
            length--;
        }
        deltaTime += data[from++];
        length--;

        if (data[from] == 0xFF) {
            runningStatus = 0;
            const int llength = data[from + 2];
            const std::vector<int> dataBytes(data.begin() + from + 3, data.begin() + from + 3 + llength);
            track.addEvent(MetaEvent(deltaTime, data[from + 1], llength, dataBytes));
            if (data[from + 1] == 0x2F) {
                break;
            }
            from += 3 + llength;
            length -= 3 + llength;
        } else if (data[from] == 0xF0) {
            runningStatus = 0;
            while (data[from] != 0xF7) {
                from++;
                length--;
            }
        } else {
            if (runningStatus >= 0x80 && data[from] < 0x80) {
                const std::vector<int> dataBytes(data.begin() + from, data.begin() + from + 2);
                track.addEvent(MidiEvent(deltaTime, runningStatus, dataBytes));
                from += 2;
                length -= 2;
            } else if (data[from] >= 0x80) {
                runningStatus = data[from];
                const std::vector<int> dataBytes(data.begin() + from + 1, data.begin() + from + 3);
                track.addEvent(MidiEvent(deltaTime, runningStatus, dataBytes));
                from += 3;
                length -= 3;
            } else {
                std::cerr << "Midi event is invalid" << std::endl;
                failFileFormat();
            }
        }
    }
}

void computeTonesFromEvents(Track &track) {
    const std::vector<std::shared_ptr<Event> > events = track.events;

    int currentMPQN = 500000;
    for (const auto &event: events) {
        if (const auto midiEvent = dynamic_cast<MidiEvent *>(event.get())) {
            const int status = midiEvent->statusByte & 0xF0;
            const int channel = midiEvent->statusByte & 0x0F;
            if (status == 0x80 || status == 0x90) {
                const bool on = status == 0x90;
                const int tone = midiEvent->dataBytes[0];
                const double frequency = computeFrequencyFromMidi(tone);
                const int velocity = midiEvent->dataBytes[1];
                const int deltaTime = computeTimeFromMidi(midiEvent->deltaTime, currentMPQN, track.ticksPerQuarter);
                track.addTone(Tone(on, channel, frequency, deltaTime, velocity));
            }
        } else if (const auto metaEvent = dynamic_cast<MetaEvent *>(event.get())) {
            if (metaEvent->typeByte == 0x51) {
                //set tempo
                if (!metaEvent->length & 0x80) {
                    currentMPQN = computeNumberFromBytes(metaEvent->dataBytes);
                }
            }
            //todo
        }
    }
}

std::vector<std::string> convertTrackToneData(Track &track) {
    std::vector<std::string> data;

    for (const auto tone: track.tones) {
        std::ostringstream oss;
        oss << "[" << tone.on << "," << tone.channel << "," << tone.frequency << ","
                << tone.deltaTime << "," << tone.velocity << "] ";
        data.push_back(oss.str());
    }

    return data;
}

bool sendSerialData(const std::vector<std::string> &data, const char* port, const int baud) {
    const int serial = open(port, O_RDWR| O_NOCTTY | O_SYNC);
    if (serial == -1) {
        std::cerr << "Error opening serial port " << port << std::endl;
        return false;
    }

    struct termios tty;
    tcgetattr(serial, &tty);
    if (baud == 115200) {
    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);
    } else {
    cfsetospeed(&tty, B9600);
    cfsetispeed(&tty, B9600);
    }

    tcsetattr(serial, TCSANOW, &tty);

    for (const auto &datan: data) {
        write(serial, datan.c_str(), datan.size());
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    write(serial, "END ", 4);

    close(serial);
    return true;

}

std::vector<char> readFileToBuffer(const std::string &filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "Unable to open file " << filename << std::endl;
        throw std::runtime_error("Unable to open file");
    }

    std::cout << "Opened file " << filename << " successfully" << std::endl;

    const std::streamsize size = file.tellg();
    if (size <= 0) {
        std::cerr << "Unable to read file " << filename << std::endl;
        failFileFormat();
    }

    file.seekg(0, std::ios::beg);
    std::vector<char> buffer(size);

    if (!file.read(buffer.data(), size)) {
        std::cerr << "Unable to read file " << filename << std::endl;
        throw std::runtime_error("Unable to read file");
    }

    file.close();
    return buffer;
}

int computeNumberFromBytes(const std::initializer_list<unsigned char> numbers) {
    int res = 0;
    for (const unsigned char n: numbers) {
        res = res * 256 + n;
    }

    return res;
}

int computeNumberFromBytes(const std::vector<int> &numbers) {
    int res = 0;
    for (const unsigned char n: numbers) {
        res = res * 256 + n;
    }

    return res;
}

int computeTimeFromMidi(const long deltaTime, const int mpqn, const int tpq) {
    return static_cast<int>(deltaTime * mpqn / (1000 * tpq));
}

double computeFrequencyFromMidi(const int note) {
    return A4 * pow(2, (note - 69) / 12.0);
}

void failFileFormat() {
    std::cerr << "File format not recognised" << std::endl;
    throw std::runtime_error("File format not recognised");
}
