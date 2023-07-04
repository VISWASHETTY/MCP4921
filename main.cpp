#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>

class MCP4921Controller : public QObject {
    Q_OBJECT
public:
    explicit MCP4921Controller(QObject *parent = nullptr);
    ~MCP4921Controller();

    void initialize();
    void setOutputVoltage(float voltage);
    void increaseInputPercentage();
    void decreaseInputPercentage();

private:
    QSerialPort *serial;
    int dac_value;

signals:
    void outputVoltageChanged(float voltage);
    void inputPercentageChanged(float percentage);

private slots:
    void readSerialData();

};

MCP4921Controller::MCP4921Controller(QObject *parent) : QObject(parent), dac_value(4095) {
    serial = new QSerialPort(this);
    connect(serial, &QSerialPort::readyRead, this, &MCP4921Controller::readSerialData);
}

MCP4921Controller::~MCP4921Controller() {
    if (serial->isOpen())
        serial->close();
    delete serial;
}

void MCP4921Controller::initialize() {
    serial->setPortName("COMx"); // Replace "COMx" with the appropriate port name
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if (!serial->open(QIODevice::ReadWrite)) {
        // Handle error opening the serial port
    }
}

void MCP4921Controller::setOutputVoltage(float voltage) {
    int outputValue = voltage * 4095 / 5;

    QByteArray data;
    data.append('v'); // Use 'v' to indicate setting voltage
    data.append(outputValue >> 8);
    data.append(outputValue & 0xFF);

    serial->write(data);

    emit outputVoltageChanged(voltage);
}

void MCP4921Controller::increaseInputPercentage() {
    if (dac_value < 4095)
        dac_value++;

    setOutputVoltage((dac_value / 4095.0) * 5.0);

    float inputPercentage = (dac_value / 4095.0) * 100.0;
    emit inputPercentageChanged(inputPercentage);
}

void MCP4921Controller::decreaseInputPercentage() {
    if (dac_value > 0)
        dac_value--;

    setOutputVoltage((dac_value / 4095.0) * 5.0);

    float inputPercentage = (dac_value / 4095.0) * 100.0;
    emit inputPercentageChanged(inputPercentage);
}

void MCP4921Controller::readSerialData() {
    QByteArray data = serial->readAll();

    // Process received data if needed
}

#include "main.moc" // Add this line to include the moc-generated implementation

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Create an instance of MCP4921Controller
    MCP4921Controller controller;

    // Create the GUI components
    QWidget window;
    QVBoxLayout layout;
    QPushButton increaseButton("Increase");
    QPushButton decreaseButton("Decrease");

    // Add the buttons to the layout
    layout.addWidget(&increaseButton);
    layout.addWidget(&decreaseButton);
    window.setLayout(&layout);

    // Connect the buttons to the controller slots
    QObject::connect(&increaseButton, &QPushButton::clicked, &controller, &MCP4921Controller::increaseInputPercentage);
    QObject::connect(&decreaseButton, &QPushButton::clicked, &controller, &MCP4921Controller::decreaseInputPercentage);

    // Show the window
    window.show();

    // Run the application event loop
    return app.exec();
}
