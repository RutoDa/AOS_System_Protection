# File System Server
> Advanced Operating Systems - Homework 2(System Protection & Distributed Synchronization)
> Homework requirements: <a src="Advanced Operating Systems - Homework 2.pdf">Advanced Operating Systems - Homework 2</a>

## Compile
### Compile Server and Client
```bash
make
```
### Clean
```bash
make clean
```

## Server Command
### server.exe
```bash
./server.exe
```

## Client Command
### client_register.exe
```bash
./client_register.exe <username> <groupname>
```

### client.exe
```bash
./client.exe
```
1. Execute the client program
2. Then enter username
#### Create a file
```bash
create <filename> <mode>
```
- Create a file with the given name and mode
  - If the file is already exist, show "File already exists"
  - If the format is not correct, show "Invalid format"
  
#### Read(Download) the file
```bash
read homework2.c
```
- Download the file from the server
  - If the file is not exist, show "File not found"
  - If the file is not readable, show "User does not have permission"
  - If someone else is writing the file, show "File is being written"
  - If the format is not correct, show "Invalid format"

#### Write(Upload) the file
```bash
write homework2.c o/a
```
- Upload the file to the server
  - If the file is not exist, show "File not found"
  - If the file is not writable, show "User does not have permission"
  - If someone else is reading or writing the file, show "File is being read or written"
  - If the format is not correct, show "Invalid format"

#### Change the mode of the file
```bash
mode homework2.c rw----
```
- Only the owner of the file can change the mode of the file
  - If the file is not exist, show "File not found"
  - If the mode is not valid, show "User is not the owner of the file"
  - If the format is not correct, show "Invalid format"

#### Exit the client
```bash 
exit
```

## Defalut Settings
### Default Users
| User      | Group        |
| --------- | ------------ |
| AOS_user1 | AOS_students |
| AOS_user2 | AOS_students |
| AOS_user3 | AOS_students |
| Ken       | AOS_students |
| Ken2      | AOS_students |
| CSE_user1 | CSE_students |
| CSE_user2 | CSE_students |
| CSE_user3 | CSE_students |
| Ken3      | CSE_students |

### Default Groups
AOS_students, CSE_students