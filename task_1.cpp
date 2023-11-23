#include <iostream>
#include <iomanip>
#include <vector>
#include <cassert>

class ReadingManager {
public:
    ReadingManager()
            : user_current_page_(MAX_USER_COUNT + 1, -1), pages_read_users_(MAX_PAGES, 0) {}

    void Read(int user_id, int page) {
        for (int i = user_current_page_[user_id] + 1; i <= page; i++) {
            pages_read_users_[i]++;
        }
        user_current_page_[user_id] = page;
    }

    double Cheer(int user_id) {
        if (user_current_page_[user_id] == -1) {
            return 0;
        }
        if (pages_read_users_[0] == 1) {
            return 1;
        }
        return double(pages_read_users_[0] - pages_read_users_[user_current_page_[user_id]]) /
               double(pages_read_users_[0] - 1);
    }

private:
    static const int MAX_USER_COUNT = 100000;
    static const int MAX_PAGES = 1000;

    std::vector<int> user_current_page_;
    std::vector<int> pages_read_users_;
};

class CommandController {
public:
    CommandController(ReadingManager &manager, std::istream &inputStream, std::ostream &outputStream)
            : manager_(manager), input_(inputStream), output_(outputStream) {}

    void HandleInput() {
        int q;
        input_ >> q;
        while (q--) {
            std::string command;
            int user_id;
            input_ >> command >> user_id;
            if (command == COMMAND_READ) {
                int page;
                input_ >> page;
                manager_.Read(user_id, page);
            } else if (command == COMMAND_CHEER) {
                output_ << std::setprecision(6) << manager_.Cheer(user_id) << std::endl;
            }
        }
    }

private:
    const std::string COMMAND_READ = "READ";
    const std::string COMMAND_CHEER = "CHEER";

    ReadingManager &manager_;
    std::istream &input_;
    std::ostream &output_;
};

void TestCommandController() {
    std::istringstream input;
    input.str(R"cmd(12
CHEER 5
READ 1 10
CHEER 1
READ 2 5
READ 3 7
CHEER 2
CHEER 3
READ 3 10
CHEER 3
READ 3 11
CHEER 3
CHEER 1)cmd");
    std::ostringstream output;
    ReadingManager rm;
    CommandController controller(rm, input, output);
    controller.HandleInput();
    std::string res = output.str();
    assert(res == "0\n1\n0\n0.5\n0.5\n1\n0.5\n");
}

int main() {
    TestCommandController();
    ReadingManager rm;
    CommandController controller(rm, std::cin, std::cout);
    controller.HandleInput();
    return 0;
}
