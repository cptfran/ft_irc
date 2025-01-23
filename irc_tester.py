import subprocess
import time

class NetcatTester:
    def __init__(self):
        self.nc_proc = None

    def start_netcat(self):
        """Start netcat to connect to IRC server in a new terminal."""
        print("Starting netcat to connect to IRC server in a new terminal...")

        # Open a new terminal to run netcat, using `-C` for CRLF mode to interpret newlines correctly
        self.nc_proc = subprocess.Popen(
            ['gnome-terminal', '--', 'bash', '-c', 'echo "Connecting to IRC server..."; nc -C localhost 6667'],
            stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True
        )
        time.sleep(1)  # Give netcat some time to start
        print("Netcat started in new terminal.")

    def send_command(self, command):
        """Send command to netcat."""
        print(f"Sending command: {command}")
        # Send the command via netcat, after the initial connection
        if self.nc_proc:
            self.nc_proc.stdin.write(command + "\n")
            self.nc_proc.stdin.flush()
            print(f"Command sent: {command}")
        else:
            print("Netcat process not started.")

    def run_irc_commands(self):
        """Run the basic IRC commands."""
        # Send the PASS, NICK, and USER commands one after another
        print("Running IRC commands...")

        # Send PASS command
        self.send_command("PASS 42")

        # Send NICK command
        self.send_command("NICK nick1")

        # Send USER command
        self.send_command("USER username placeholder placeholder :realname")

    def stop_netcat(self):
        """Stop the netcat process when done."""
        print("Stopping netcat...")
        if self.nc_proc:
            self.nc_proc.stdin.write("/quit\n")
            self.nc_proc.stdin.flush()
            self.nc_proc.wait()
            print("Netcat stopped.")

if __name__ == '__main__':
    tester = NetcatTester()

    # Start netcat session
    tester.start_netcat()

    # Give netcat some time to connect and process
    time.sleep(2)  # Wait for netcat to connect and initialize

    # Run IRC commands
    tester.run_irc_commands()

    # Wait a little bit before closing netcat
    time.sleep(5)

    # Stop netcat after the commands are executed
    tester.stop_netcat()
