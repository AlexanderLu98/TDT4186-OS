/*3.3 I/O redirection
Implement simple I/O redirection for stdin and stdout when this is indicated on the command line using the < or >
characters followed by a file name.
You can simplify this task by only accepting input and output redirections as the last parameters of a command line
(excluding the “&” character for background processes, see below), e.g.:
/home/user/shell: ls > /tmp/foo
Exit status: 0
/home/user/shell: head -1 < /etc/passwd
root:*:0:0:System Administrator:/root:/bin/sh
Exit status: 0
/home/user/shell: head -1 < /etc/passwd > /tmp/foo2
Exit status: 0
*/
