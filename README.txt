Step to build virtual RTMBackend in server 10.158.14.149

Step 1: Create folder rtm-server
a. Go to \\10.158.14.149\hung.hoang\a35\wave
b. Type: mkdir rtm-server

Step 2: Copy file server.sh to folder rtm-server

Step 3: Create folder bin
a. cd rtm-server
b. mkdir bin

Step 4: Copy all source code in local file D:\hunght\Wave_TPM4G\3.SourceCode\Virtual_Server\rtm-server 
to 
\\10.158.14.149\hung.hoang\a35\wave\rtm-server\bin

Step 5: Create bitbake file
a. Go to \\10.158.14.149\hung.hoang\a35\sources\meta-lge-wave\recipes-mgrs
b. Create folder rtm-server
c. Copy file rtm-server.bb to \\10.158.14.149\hung.hoang\a35\sources\meta-lge-wave\recipes-mgrs\rtm-server

Step 6: Build and run
a. Go to \\10.158.14.149\hung.hoang\a35\wave\rtm-server
b. Run server.sh
c. Build result in folder: \\10.158.14.149\hung.hoang\a35\build\tmp\work\aarch64-poky-linux\rtm-server\1.0-r0\build