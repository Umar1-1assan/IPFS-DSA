# IPFS Project - DSA Concepts Implementation

This project implements a distributed file storage system using the InterPlanetary File System (IPFS) while incorporating core Data Structures and Algorithms (DSA) concepts. The system leverages **SHA1** for file handling and uses **B-trees** to provide an optimized way to store and retrieve files efficiently.

## Features:
- **SHA1**: We use the SHA1 hash function to uniquely identify files in the system. This ensures that each file is stored in the network with a distinct identifier based on its content, ensuring data integrity and efficient retrieval.
- **B-tree for Optimized Storage**: The project uses B-trees for file storage, providing an efficient mechanism for storing and indexing files. This allows fast search, insertion, and deletion operations while minimizing the number of disk accesses, making the system scalable and responsive.
- **Folder Structure for Machines**: To visualize the concepts of file storage and management, we create folders for each machine within the system. These folders represent different machines or nodes in the distributed network, where files are uploaded, stored, and retrieved.
- **File Upload to Folders**: Users can upload files to these folders, demonstrating how data can be organized and accessed within a distributed system. The uploaded files are stored using their unique SHA1 hash, ensuring efficient management and retrieval.
  
## DSA Concepts:
- **SHA1**: The system uses SHA1 hashes to generate unique identifiers for each file. This technique ensures that even if two files are identical, they will share the same hash value, making deduplication easier and enhancing the integrity of stored data.
- **B-tree**: A balanced tree structure that optimizes the process of searching, inserting, and deleting files. B-trees are ideal for managing large datasets, and their use in this project ensures that file access remains efficient even as the number of files in the system grows.

## Technologies Used:
- **IPFS**: For distributed file storage and retrieval.
- **SHA1 Hashing**: To uniquely identify files.
- **B-tree**: For optimized file storage and management.
- **Python**: For implementing the project and visualizing the concepts.

## Visualizing DSA Concepts:
To help visualize how files are stored and managed, we use code to simulate the creation of folders for different machines or nodes. Files are uploaded to these folders and indexed using SHA1 hashes. The B-tree structure ensures efficient file access and management across the network, demonstrating the practical application of these DSA concepts in real-world scenarios.

## Getting Started:
1. Clone the repository.
2. Install required dependencies for Python and IPFS.
3. Run the script to initialize the system and begin uploading files to the network.
  
## License:
This project is licensed under the MIT License - see the LICENSE file for details.
