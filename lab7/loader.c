/********************************************************
Luke Darrow 11349190
CptS 460
Loader.c
********************************************************/

#define BLK 1024

//buffers for blocks
char b1[BLK], b2[BLK];

//name buffer
char *name[32];

//path buffer
char path[32];

u16 tsize, dsize, bsize, total_size;
//number of names
u16 num_names;

//header struct
typedef struct h{
	u32 ID_space;         // 0x04100301:combined I&D or 0x04200301:separate I&D
	u32 magic_number;     // 0x00000020
	u32 tsize;            // code section size in bytes
	u32 dsize;            // initialized data section size in bytes
	u32 bsize;            // bss section size in bytes
	u32 zero;             // 0
	u32 total_size;       // total memory size, including heap
	u32 symbolTable_size; // only if symbol table is present
} HEADER;

//get block function from assignment page
//loads a disk block number, blk, into char buf[1024]
u16 get_block (u16 blk, char *buf)
{
	//convert blk into (C,H,S) format by Mailman to suit disk geometry
	//      CYL         HEAD            SECTOR
	diskr( blk/18, ((2*blk)%36)/18, (((2*blk)%36)%18), buf);
}

//NOTE: block numbers in INODEs are u32 values. You must Call
//      the get_block() function with blk of the right size.

//search the blocks for name
u16 search(INODE *ip, char *name)
{
	int i;//index
	char c;
	DIR *dp;//directory pointer

	//there's 12 blocks
	for (i = 0; i < 12; i++)
	{
		//make sure there's something there
		if( (u16)ip->i_block[i] )
		{
			//get block into buffer 2
			get_block((u16)ip->i_block[i], b2);

			//set the directory pointer by type casting buffer 2
			dp = (DIR *) b2;

			//iterate through directories
			while ((char *)dp < &b2[BLK])
			{
				//save the last character since we're going to remove it
				c = dp->name[dp->name_len];

				//remove the last byte since it will ruin our compare
				dp->name[dp->name_len] = 0;

				//print directories for testing purposes
				//printf("%s\n", dp->name);

				//if the name matches, we've found it!
				if ( strcmp(dp->name, name) == 0 )
				{
					//return the inode of the directory we were looking for
					return((u16)dp->inode);
				}

				//restore the last character that we removed earlier
				dp->name[dp->name_len] = c;

				//go to the next directory
				dp = (char *)dp + dp->rec_len;
			}
		}
	}

	//couldn't find it, return 0
	return 0;
}

//tokenize the path and stores the elements in name[]
int tokenize(char *path)
{
	int i;
	char *tok;

	//set global counter for the number of elements in the path to 0
	num_names = 0;

	printf("path is %s\n", path);
	if(*path == '/')
			*path++;
	tok = strtok(path, "/");
	name[num_names++] = tok;

	//keep going until the end of cp (the path)
	while(*path != 0)
	{
		tok = strtok(0, "/");
		if(tok == 0)
			break;
		name[num_names++] = tok;
	}

	//print array for testing purposes
	//for(i = 0; i < num_names; i++)
	//	printf("name[%d] = %s ", i, name[i]);
	//printf("\n");
}

int load(char *filename, u16 segment)
{
	u16 i, j, word;
	u16 name_ino, blk, iblk;
	u16 seg, tdsize, rem;
	u32 *up;
	INODE *ip;
	GD *gp;
	HEADER *hp;

	//copy the filename into path so we don't destroy it
	strcpy(path, filename);
	//tokenize the path using / as delimiter
	tokenize(path);

	//load group descriptor block into buffer 1
	get_block(2, b1);
	//set group descriptor
	gp = (GD *)b1;

	//set up the inode table from the group descriptor
	iblk = (u16)gp->bg_inode_table;

	//load the first inode block into buffer 1
	get_block(iblk, b1);
	//set inode pointer to block 1 + 1
	ip = (INODE *)b1 + 1;

	// 1) find the inode of filename. return 0 if fails.

	//search for the system name
	for (i = 0; i < num_names; i++)
	{
		//ino of name we look for in search
		name_ino = search(ip, name[i]);
		if (name_ino == 0)//if == 0, not found
		{
			//print error message, return 0
			printf("%s does not exist!\n", name[i]); 
			return 0;
		}

		name_ino--;
		//mailman into system name's ino
		get_block(iblk + (name_ino / 8), b1);
		ip = (INODE *)b1 + (name_ino % 8);
	}

	// 2) read file header to get tsize, dsize and bsize
	//load header into buffer 2
	get_block((u16)ip->i_block[0], b2);

	//set header pointer from buffer 2
	hp = (HEADER*) b2;

	//set global header vars
	tsize = hp->tsize;
	dsize = hp->dsize;
	bsize = hp->bsize;
	total_size = hp->total_size;

	// 3) load [code|data] sections of filename to memory segment

	//if indirect blocks, get_block indirect into buffer 2
	if (ip->i_block[12])
		get_block((u16)ip->i_block[12], b2);

	printf("loading %s to segment 0x%x ....", filename, segment);
	//ES must be set by setes(segment) function to a desired loading segment before calling readfd()
	setes(segment);

	//load indirect blocks (blocks 0-11)
	for (i=0; i<12; i++)
	{
		//check if empty. don't get_block an empty blk
		if (ip->i_block[i]==0) 
			break;
		
		get_block((u16)ip->i_block[i], 0);
		//increment ES by NSEC sectors (in 16-byte clicks) to load the next NSEC sectors
		inces();
	}

	// load indirect blocks if any
	if ( (u16)ip->i_block[12] )
	{
		up = (u32 *) b2;

		while( *up )
		{
			//get the block
			get_block((u16)*up, 0);
			//increment ES by NSEC sectors (in 16-byte clicks) to load the next NSEC sectors
			inces();
			//increment pointer to next part of block
			up++;
		}
	}
   
	// move the image upwards 32 bytes
	for (i = 0; i <= tsize + dsize ; i += 2)
	{
		word = get_word(segment + 2, i);
		put_word(word, segment, i);
	}

	// 4) clear bss section of loaded image to 0;

	tdsize = tsize + dsize;
	seg = segment + tdsize / 16;

	rem = tdsize % 16;

	for (i = 0; i < rem; i++)
		put_byte(0, seg, i);

	//use j since we're starting where i left off
	for (j = 0; j < bsize; j++)
		put_byte(0, seg, j + i);

	//ES must be set by setes(segment) function to a desired loading segment before calling readfd()
	//restore ES to the MTX segment
	setes(0x1000);
	printf(" done\n");

	// 5) return 1 for success
	return 1;
}
