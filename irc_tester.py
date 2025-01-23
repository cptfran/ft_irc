import argparse
import subprocess

class IrcTester:
    def __init__(self, num_of_terminals):
        self.num_of_terminals = num_of_terminals

    def launch_terminals(self):
        for _ in range(self.num_of_terminals):
            # Launch each terminal and execute the command
            subprocess.Popen([
                'gnome-terminal',
                '--',
                'bash', '-c', 'nc -C localhost 6667'
            ])
        print(f"Launching {self.num_of_terminals} terminals...")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Launch GNOME terminals.")
    parser.add_argument("num_of_terminals", type=int, help="Number of GNOME terminals to launch")
    args = parser.parse_args()

    tester = IrcTester(args.num_of_terminals)
    tester.launch_terminals()
