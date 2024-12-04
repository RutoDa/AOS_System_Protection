
## Client Command
### Client.exe

#### Create a file
```bash
create <filename> <mode>
```
- Create a file with the given name and mode
  - If the file is already exist, show "File already exists"
  - If the file is not exist, create a new file with the given name and mode
  - If the mode is not valid, show "Invalid mode"
  - If the format is not correct, show "Invalid format"
  
#### Read(Download) the file
```bash
read homework2.c
```
- Download the file from the server
  - If the file is not exist, show "File not found"
  - If the file is not readable, show "Permission denied"

#### Write(Upload) the file
```bash
write homework2.c o/a
```

#### Change the mode of the file
```bash
mode homework2.c rw----
```

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
| CSE_user1 | CSE_students |
| CSE_user2 | CSE_students |
| CSE_user3 | CSE_students |

### Default Groups
AOS_students, CSE_students