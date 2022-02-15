jay ram
jcr2211
lab5

===PART 1===
b)

3140 32249 32249 32249 ?           -1 Ss       0   0:00  \_ sshd: jcr2211 [priv]
32249 32515 32249 32249 ?           -1 S     4781   0:00  |   \_ sshd: jcr2211@pts/116
32515 32516 32516 32516 pts/116   7373 Ss    4781   0:00  |       \_ -bash
32516 18744 18744 32516 pts/116   7373 T     4781   0:00  |           \_ /bin/sh ./mdb-lookup-server-nc-1.sh 33333
18744 18747 18744 32516 pts/116   7373 T     4781   0:00  |           |   \_ nc -l 33333
32516  7373  7373 32516 pts/116   7373 S+    4781   0:00  |           \_ ./mdb-lookup-server-nc-1 5869
 7373  7374  7373 32516 pts/116   7373 S+    4781   0:00  |               \_ /bin/sh ./mdb-lookup-server-nc.sh 5869
 7374  7376  7373 32516 pts/116   7373 S+    4781   0:00  |                   \_ cat mypipe-7374
 7374  7377  7373 32516 pts/116   7373 S+    4781   0:00  |                   \_ nc -l 5869
 7374  7378  7373 32516 pts/116   7373 S+    4781   0:00  |                   \_ /bin/sh /home/jae/cs3157-pub/bin/mdb-lookup-cs3157
 7378  7379  7373 32516 pts/116   7373 S+    4781   0:00  |                       \_ /home/jae/cs3157-pub/bin/mdb-lookup /home/jae/cs3157-pub/bin/mdb-cs3157

the shell scripts end with .sh:
mdb-lookup-server-nc.sh
mdb-lookup-server-nc-1.sh

c)


