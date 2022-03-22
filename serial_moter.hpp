#include <string>
#include <vector>

class serial_moter
{
    public:
        // Opens a communication with the serial_moter in serial port 'port'.
        // Starts the robot in Safe Mode
        serial_moter(std::string port);

        // Destructor: Closes the serial port
        ~serial_moter();

        void moveAngle(int angle);
        void openloop(int power);
        void openloop_demo();
        void openloop_zero();

        // Sends commands to the robot. Each element of the vector is a command.
        void sendCommands(std::vector<uint8_t> commands);

    protected:
    private:
        // Sends/receives information through serial connection
        int serialOutput;
};
