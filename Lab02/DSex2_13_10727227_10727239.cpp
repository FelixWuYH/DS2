// 10727227 �B�a�� 10727239 �|�§�   "�ЧU�ФU��GitHub��������"
/*���Ȥ@�G 
     �ثesplitNonLeaf�i���٦��@�ǰ��D
  ���ȤG:
  	 insert���@�ǰ��D  
*/ 
# include <iostream>
# include <fstream>
# include <string>
# include <vector>
# include <cstdlib>

# include <stack> // for Mission1
# define PTR_NUM 3
# define KEY_NUM PTR_NUM - 1

using namespace std ;

typedef struct sT {
	int order ;
	string sname ;
	string dname ;
	string type ;
	string level ;
	int ngrad ;
} schoolType ;

class SchoolList {
	private :		
		vector<schoolType> allR ;
		string fileID ;
		
		void reset() {
			this->allR.clear() ;
			this->fileID.clear() ;
		} // end reset

		
	public :
		SchoolList() {
			 reset() ;
		}

		
		~SchoolList() {
			reset() ;
		}

		
		bool readFile() {
			fstream inFile ;
			string fileName ;
			
			this->reset() ;
			cout << endl << "Input a file number: " ;
			cin >> this->fileID ;
			fileName = "input" + fileID + ".txt" ;
			inFile.open( fileName.c_str(), fstream::in ) ;
			if ( !inFile.is_open() ) {
				cout << endl << "###" << fileName << "does not exist! ###" << endl ;
				return false ;
			} // if
			else { 
				char cstr[255] ;
				int fNo, pre, pos ;
				
				inFile.getline( cstr, 255, '\n' ) ;
				inFile.getline( cstr, 255, '\n' ) ;
				inFile.getline( cstr, 255, '\n' ) ;
				int num = 1 ;
				while ( inFile.getline( cstr, 255, '\n' ) ) {
					schoolType oneR ;
					string buf, cut ;
					
					fNo = 0 ;
					pre = 0 ;
					buf.assign( cstr ) ;
					do {
						pos = buf.find_first_of( '\t', pre ) ;
						cut = buf.substr( pre, pos - pre ) ;
						switch ( ++fNo ) {
							case 2 :
								oneR.sname = cut ;
								break ;
							case 4 :
								oneR.dname = cut ;
								break ;
							case 5 :
								oneR.type = cut ;
								break ;
							case 6 :
								oneR.level = cut ;
								break ;
							case 9 :
								oneR.ngrad = atoi( cut.c_str() ) ;
								break ;
							default :
								break ;
						} // end switch
						
						pre = ++pos ;
					} while ( (pos > 0 ) && ( fNo < 10 ) ) ;
					oneR.order = num ;
					num = num + 1 ;
					this->allR.push_back( oneR ) ;
				} // while
				
				inFile.close() ;
			} // end else
			
			if ( !this->allR.size() ) {
				cout << endl << "### Getnothing from the file " << fileName << "!###" << endl ;
				return false ;
			} // end if
			
			return true ;
		} // end readFile

		void PrintAllR() {
			int i = 0 ;
			schoolType temp ;
			while ( allR.size() > i ) {
				temp = allR.at(i) ;
				cout << "[" << temp.order << "]" << "\t" << temp.sname << "\t" << temp.dname << "\t"
				<< temp.type << "\t"<< temp.level << "\t" << temp.ngrad << endl ;
				i =  i + 1 ;
			} // while
			
		} // PrintAllR()

		vector<schoolType> GetData() {
			return allR ;
		} // GetData()
		
} ; // SchoolList

class TwoThreeTree {
	private :				
		typedef struct slotT { // a slot in a tree node ��b�@�Ӹ`�I������� 
			vector<int> rSet ; // a set of record identifiers wth the same key �ǮզW�٬ۦP���Ǹ� 
			string key ; // a key for comparisons �ǮզW�� 
		} slotType ;
		
		typedef struct nT { // a tree node of a 23 tree �@�Ӹ`�I 
			slotType data[ KEY_NUM ] ; // a list of records sorted by keys ��ơA�Ѥp��j�ƦC (�@�Ӹ`�I�̦h2��) 
			struct nT *link[ PTR_NUM ] ; // a list of pointers ���V���U�A�����A�k�U (�@�Ӹ`�I�̦h3��) 
			struct nT *parent ;  // a pointer to the parent node ���V���`�I 
		} nodeType ;
		
		typedef struct pointT { // a point on the search path ���|�i�ਫ�L�Y�A��stack�^�ӧ� 
			nodeType *pnode ; // pointer to a parent node
			int pidx ; // entrance index on the parent node
		} pointType ;
		
		typedef struct bT { // a data block received from a split �����X����� 
			slotType slot ; // a pair of( record id, key )
			nodeType *link ; // a pointer to a child on the right ���V�k�䪺�l�`�I 
		} blockType ;
		
		nodeType * root ; // ���V23�𪺮� 
		
		void insert23tree( int newRid, string newKey, nodeType *&root ) { // add one record into 23 tree ��J�@�ӷs����� 
			// input: a new record passed as a pair of ( newKey, newRid ) ,tree root 
			// output: tree root after update
			
			slotType newSlot ; // �N�Ǹ��M�զW��J 
			newSlot.rSet.push_back( newRid ) ; // the content of a new record
			newSlot.key = newKey ;
			
			if ( root == NULL )  { // create the first node with one record inserted �ڤ��s�b��
				root = createNode( NULL, NULL, NULL, newSlot ) ; // �ؤ@�ӷs����� [END!]
			} // if
			else { // the root exists �ڦs�b��
				stack<pointType> aPath ; // stack to keep the searh path �O�����| 
				pointType curP ; // last-visited node at the top of stack ���|���̤W��A���̫��F�����| 
				blockType blockUp ; // a data block received from a split �����ɥΨӱ��� 
				
				searchPath( root, newKey, aPath ) ; // find a matched position on 23 tree ��A���a��
				if ( !aPath.empty() ) { // �w�����| 
					curP = aPath.top() ; // reference to the last-visited node ��stack�̤W�誺�`�I
					
					if ( ( curP.pnode->data[ curP.pidx ].rSet.size() ) && ( !newKey.compare( curP.pnode->data[ curP.pidx ].key ) ) )
						// it's a duplicate key, so insert it directly ���ǮզW�٤@�˪��ɭ� 
						curP.pnode->data[ curP.pidx ].rSet.push_back( newRid ) ; // add a new record identifier
						// �N�Ǹ���Jvector(�̧ǩ�J�G�����Ƨ�) [END!]
					else if ( !curP.pnode->data[ KEY_NUM - 1 ].rSet.size() ) // ���l���˺���ơA�u���@���� 
						insertLeaf( newSlot, curP ) ; // add a record into a leaf ���l�[�J��� 
						// at least one ( rightmost ) unused slot [END!]
					else { // split a full leaf ���l�w������� 
						splitLeaf( newSlot, curP, blockUp ) ; // split a leaf for an insertion �������@��(���᩹�W���i��]�n����)
						
						if ( curP.pnode->parent == NULL ) // if a root is split, creat a new root ��ڳQ�����n���ͷs����� 
							root = createRoot( curP.pnode, blockUp.link, blockUp.slot ) ; // left, right, oneslot [END!]
						else // continue splitting a non-leaf for an insertion �ˬd�O�_�n�����B�W�観�`�I 
							do {
								aPath.pop() ; // forget the current node
								curP = aPath.top() ; // the next parent for an insertion ���V�e�@�B(���W�@�h) 
								
								if ( !curP.pnode->data[ KEY_NUM - 1 ].rSet.size() ) { // at least one ( rightmost ) unused slot �`�I������ƥu���@�� 
									insertNonleaf( blockUp, curP ) ; // add a slot into a non-leaf [END!]
									break ; // finish the insertion
								} // end inner if
								else { 
									splitNonleaf( curP, blockUp ) ; // split a non-leaf for an insertion
									
									if( curP.pnode->parent == NULL ) { // if a root is split, create a new root
										root = createRoot( curP.pnode, blockUp.link, blockUp.slot ) ; // left, right, oneslot
										break ; // finish the insertion!
									} // end very inner if
									
								} // end inner else
							} while ( true ) ; // repeat until the insertion is terminated
					} // end moddle else
				} // end outer if 
			} // end outer else
		} // end insert23tree
		
		nodeType *createNode( nodeType *left, nodeType *right, nodeType *pNode, slotType newS ) { // create a node with one record inserted
			// input: left-child, right-child, parent, a new recordkept as a slot �ؤ@�ӷs���`�I 
			// output: a new-created node or NULL
			
			nodeType *newNode = NULL ;
			
			try {
				newNode = new nodeType ; // create a new node �ؤ@�ӷs���`�I
				
				newNode->data[0].rSet = newS.rSet ; // put the record into the 1st slot �u���@����� 
				newNode->data[0].key = newS.key ;
				
				newNode->data[1].rSet.clear() ;
				newNode->data[1].key = "" ;
				
				newNode->link[0] = left ; // set up the leftmost link �طs�`�I�u�����k����� 
				newNode->link[1] = right ; // set up the middle line
				
				newNode->link[2] = NULL ; // clear up the rightmost link
				
				newNode->parent = pNode ; // set up a link to the parent
			} // end try
			catch ( std::bad_alloc & ba ) { // unable to allocate space
				std::cerr << endl << "bad_alloc caught:" << ba.what() << endl ;
			} // end catch
			
			return newNode ; // pass a pointer to the new-created node
		} // end createNode
		
		void searchPath( nodeType *cur, string name, stack<pointType> path ) { // find a matched slot or the position to insert ������쪺��Ʃι諸�a�� 
			// input: the root, a name
			// output: the search path �u������|�Ӥw 
			
			pointType oneP ;
			int pos ;
			while ( cur != NULL ) { // ��ܲ{�`�I�٦���� 
				oneP.pnode = cur ; // �O���{�b���`�I 
				for ( pos = 0 ; pos < KEY_NUM ; pos++ ) // KEY_NUM is 2
					if ( !cur->data[pos].rSet.size() || // unused slot, name > key
						( ( name.compare( cur->data[pos].key ) ) < 0 ) ) // or name < key �ΦW�r����p��
						break ; // search the next level �O���O����p����(0)
					else if ( !name.compare( cur->data[pos].key ) ) { // name == key ( a duplicate! ) �W�r�ۦP�� 
						oneP.pidx = pos ; // keep track of the pointer �O���O����p����(0)�Τ���j����(1) 
						path.push( oneP ) ; // visited node: ( parent node, entrance index ) ��J���| 
						return ; // the last-visited node is at the top of stack ���]�����}�禡 
					} // end else if
				
				oneP.pidx = pos ; // keep track of the pointer �O���O����p����(0)�Τ���j����(1) 
				path.push( oneP ) ; // visited node: ( parent node, entrance index ) ��J���| 
				cur = cur->link[pos] ; // recursive search at the next level ���U�@�h�� 
			} // end while
		} // end searchPath
		
		void insertLeaf( slotType newS, pointType aLeaf ) { // add a record into a leaf �[�J�@����ƨ츭�`�I 
			// input: a new slot( rSet, key ), the leaf to insert ( pnode, pidx )
			// output: leaf after update
			
			for ( int i = KEY_NUM - 1 ; i >= aLeaf.pidx ; i-- ) // scan from right to left ( i starts from 1 )
				if ( i > aLeaf.pidx ) { // shift an existing record to the right ( 1 > 0 )�ɡA�����k��j�A�]�����䩹�k�� 
					aLeaf.pnode->data[i].rSet = aLeaf.pnode->data[ i - 1 ].rSet ;
					aLeaf.pnode->data[i].key = aLeaf.pnode->data[ i - 1 ].key ;
				} // end if
				else if ( i == aLeaf.pidx ) { // ( 1 == 1 ) or ( 0 == 0 )
					aLeaf.pnode->data[i].rSet = newS.rSet ; // save the new record in a new slot
					aLeaf.pnode->data[i].key = newS.key ;
				} // end else if
				else
					break ; // earlier termination
		} // end insertLeaf
		
		void splitLeaf( slotType newS, pointType aLeaf, blockType & aBlock ) { // split a leaf for an insertion �����@�Ӹ��`�I 
			// input: a new slot( rSet, key ), the leaf to insert( pnode, pidx )
			// output: block after split to move upwards
			
			slotType buf[ PTR_NUM ] ; // a buffer to keep a full node plus a new record �Ȧs�T�Ӹ�� 
			int idx = 0 ; // index of the full node
			for ( int i = 0 ; i < PTR_NUM ; i++ ) { // fill in the entre buffer // ��J�Ȧs�����A( PTR_NUM = 0 ~ 2 ) [0], [1], [2] 
				buf[i].rSet = ( i == aLeaf.pidx ) ? ( newS.rSet ) : ( aLeaf.pnode->data[idx].rSet ) ;
				buf[i].key = ( i == aLeaf.pidx ) ? ( newS.key ) : ( aLeaf.pnode->data[idx++].key ) ; // idx++ => (1.) [idx] (2.) idx = idx + 1 
			} // end for
			
			aLeaf.pnode->data[0].rSet = buf[0].rSet ; // leave only the leftmost record �u�d�U�̥��䪺 
			aLeaf.pnode->data[0].key = buf[0].key ;
			
			for ( int i = 1 ; i < KEY_NUM ; i++ ) { // the remains unused slots 
				aLeaf.pnode->data[i].rSet.clear() ;
				aLeaf.pnode->data[i].key = "" ;
			} // for
				
			aBlock.link = createNode( NULL, NULL, aLeaf.pnode->parent, buf[2] ) ; // create a sibling node on the right �b�k��ؤ@�ӷs�`�I�A�ë��V��			
			aBlock.slot.rSet = buf[1].rSet ; // block to move upwards 
			aBlock.slot.key = buf[1].key ;
			
		} // end splitLeaf
		
		nodeType *createRoot(nodeType * left, nodeType * right, slotType oneSlot ) { // create a new root after a split �طs��� 
			// input: left child, right child, one slot in the new root
			// output: a new root in a well-connected tree
			nodeType *newRoot = createNode( left, right, NULL, oneSlot ) ;
			
			left->parent = newRoot ; // set their parent as the new root
			right->parent = newRoot ;
			return newRoot ;
		} // end createRoot
		
		void insertNonleaf( const blockType oneB, pointType goal ) { // add a record into a leaf
			// input: a new block( slot, link ), the non-leaf to insert( pnode, pidx )
			// output: non-leaf after update
			
			// Mission One. part1
			// find the position on goal.pnode to insert oneB
			// 1. shift an existing record to the right if it is the 1st positon
			// 2. add a new record and its right child into the position
			
			if ( oneB.slot.key < goal.pnode->data[0].key ) { // insert the record to the right position
							
				goal.pnode->data[1].rSet = goal.pnode->data[0].rSet ;
				goal.pnode->data[1].key = goal.pnode->data[0].key ;
				
				goal.pnode->data[0].rSet.clear() ;
				goal.pnode->data[0].key = "" ;
				
				goal.pnode->data[0].rSet = oneB.slot.rSet ;
				goal.pnode->data[0].key = oneB.slot.key ;
			} // if
			else if ( oneB.slot.key > goal.pnode->data[0].key ) {
				goal.pnode->data[1].rSet = oneB.slot.rSet ;
				goal.pnode->data[1].key = oneB.slot.key ;
			} // else if
			
			if ( oneB.slot.key > goal.pnode->data[1].key ) { // change the link to the right position
				goal.pnode->link[2] = oneB.link ;
			} // if
			else if ( oneB.slot.key > goal.pnode->data[0].key ) {
				goal.pnode->link[2] = goal.pnode->link[1] ;
				goal.pnode->link[1] = oneB.link ;				
			} // else if
			else {
				goal.pnode->link[2] = goal.pnode->link[1] ;
				goal.pnode->link[1] = goal.pnode->link[0] ;
				goal.pnode->link[0] = oneB.link ;
			} // else
			
		} // end insertNonleaf
		
		void splitNonleaf( pointType goal, blockType & oneB ) { // split a non-leaf for an insertion
			// input: the leaf to insert( pnode, pidx )
			// input/output: block before and after split
			
			slotType buf[ PTR_NUM ] ; // a buffer to keep a full ndoe plus a new record buf[0], buf[1], buf[2]
			nodeType * ptr[ PTR_NUM  + 1 ] ; // a buffer to keep pointers of children ptr[0], ptr[1], ptr[2], ptr[3]
			int idx = 0 ; // index of the full node
			
			for ( int i = 0 ; i < PTR_NUM ; i++ ) { // fill in the entire buffer �Ȧs�T�Ӹ�� 
				buf[i].rSet = ( i == goal.pidx ) ? ( oneB.slot.rSet ) : ( goal.pnode->data[idx].rSet ) ;
				buf[i].key = ( i == goal.pidx ) ? ( oneB.slot.key ) : ( goal.pnode->data[idx++].key ) ;
			} // for
			
			// Mission One. part2
			// put the sorted records in buf into three places: goal.pnode, oneB, a new sibiling node
			// 1. fill in the pointers in correct order
			// 2. put only the smallest record on goal.pnode
			// 3. create a new sibling node and leave only the largest record on it
			
			if ( oneB.link->data[0].key > goal.pnode->link[2]->data[0].key ) {
				ptr[0] = goal.pnode->link[0] ;
				ptr[1] = goal.pnode->link[1] ;
				ptr[2] = goal.pnode->link[2] ;
				ptr[3] = oneB.link ;
			} // if
			else if ( oneB.link->data[0].key > goal.pnode->link[1]->data[0].key ) {
				ptr[0] = goal.pnode->link[0] ;
				ptr[1] = goal.pnode->link[1] ;
				ptr[2] = oneB.link ;
				ptr[3] = goal.pnode->link[2] ;				
			} // if
			else if ( oneB.link->data[0].key > goal.pnode->link[0]->data[0].key ) {
				ptr[0] = goal.pnode->link[0] ;
				ptr[1] = oneB.link ;
				ptr[2] = goal.pnode->link[1] ;
				ptr[3] = goal.pnode->link[2] ; 				
			} // if
			else {
				ptr[0] = oneB.link ;
				ptr[1] = goal.pnode->link[0] ;
				ptr[2] = goal.pnode->link[1] ;
				ptr[3] = goal.pnode->link[2] ;				
			} // else
			
			
			goal.pnode->data[0].rSet = buf[0].rSet ;
			goal.pnode->data[0].key = buf[0].key ;
			
			goal.pnode->data[1].rSet.clear() ;
			goal.pnode->data[1].key = "" ;
			
			nodeType *newNode = createNode( ptr[2], ptr[3], goal.pnode->parent, buf[2] ) ;			
			
			oneB.slot.rSet = buf[1].rSet ; // block  to move upwards
			oneB.slot.key = buf[1].key ;
			oneB.link = newNode ;
		} // end splitNonleaf

	public :
		void BuildTree( vector<schoolType> input ) {
			root = NULL ; // �@�}�l���ګ��Vnull 
			for ( int i = 0 ; i < input.size() ; i++ ) {
				insert23tree( input.at(i).order, input.at(i).sname, root ) ;
			} // for
		/*
			for ( int i = 0 ; i < root->data[0].rSet.size() ; i++ ) {
				cout << root->data[0].key << endl ;
			} // for
		*/	
		} // BuildTree()
	
		int GetTreeHeight() {
			int i = 0 ;
			CountHeight( root, i ) ;
			return i ;
		} // GetTreeHeight()
		
		int CountHeight( nodeType *cur, int &i ) {
			if ( cur == NULL ) {
				cout << "NULL" << endl ;
				;
			} // if
			else if ( !cur->data[ KEY_NUM ].rSet.size() ) {
				cout << "left" << endl ;
				i = i + 1 ;
				CountHeight( cur->link[0], i ) ;
				CountHeight( cur->link[1], i ) ;
				CountHeight( cur->link[2], i ) ;
			} // else if
			else if ( !cur->data[ KEY_NUM - 1 ].rSet.size() ) {
				cout << "right" << endl ;
				i = i + 1 ;
				CountHeight( cur->link[0], i ) ;
				CountHeight( cur->link[1], i ) ;
			} // else if
			
		} // CountHeight()
		
} ;

class AVL {
	private :
		typedef struct node {
			vector<int> order ;
			int graduated ;
			node *left ;
			node *right ;
			int height ;
		} nodeType ;
		
		nodeType *root ;
		
		nodeType *RotateLL( nodeType *x ) {
			nodeType *y = x -> left ;
			x -> left = y -> right ;
			y -> right = x ;
			
			x -> height = max( Height( x -> left ), Height( x -> right ) ) + 1 ;
			y -> height = max( Height( y -> left ), Height( y -> right ) ) + 1 ;
			return y ;
		} // LL()
		
		nodeType *RotateRR( nodeType *x ) {
			nodeType *y = x -> right ;
			x -> right = y -> left ;
			y -> left = x ;
			
			x -> height = max( Height( x -> left ), Height( x -> right ) ) + 1 ;
			y -> height = max( Height( y -> left ), Height( y -> right ) ) + 1 ;
			return y ;
		} // RR()
				
		nodeType *RotateRL( nodeType *x ) {
			x -> right = RotateLL( x -> right ) ;
			return RotateRR( x ) ;
		} // RL()
		
		nodeType *RotateLR( nodeType *x ) {
			x -> left = RotateRR( x -> left ) ;
			return RotateLL( x ) ;
		} // LR()
		
		nodeType *createNode( nodeType *left, nodeType *right, int order, int ngrad ) {
			nodeType *newNode = new nodeType() ;
			
			newNode -> order.push_back( order ) ;
			newNode -> graduated = ngrad ;
			newNode -> height = 0 ; 
			newNode -> left = left ;
			newNode -> right = right ;
			
			return newNode ;
		} // createNode()
		
		nodeType *insert( nodeType *&pnode, int order, int ngrad ) {
			if ( pnode == NULL ) {
				pnode = createNode( NULL, NULL, order, ngrad ) ;
			} // if
			else if ( pnode -> graduated < ngrad ) {
				pnode -> right = insert( pnode -> right, order, ngrad ) ;
				if ( Height( pnode -> right ) - Height( pnode -> left ) == 2 ) {
					if ( pnode -> right -> graduated < ngrad ) {
						pnode = RotateRR( pnode ) ;
					} // if
					else if ( pnode -> right -> graduated > ngrad ) {
						pnode = RotateRL( pnode ) ;
					} // else if
				} // if
			} // else if
			else if ( pnode -> graduated > ngrad ) {
				pnode -> left = insert( pnode -> left, order, ngrad ) ;
				if ( Height( pnode -> left ) - Height( pnode -> right ) == 2 ) {
					if ( pnode -> left -> graduated < ngrad ) {
						pnode = RotateLL( pnode ) ;
					} // if
					else if ( pnode -> left -> graduated > ngrad ) {
						 pnode = RotateLR( pnode ) ;
					} // else if
				} // if
			} // else if
			else if ( pnode -> graduated == ngrad ) {
				pnode -> order.push_back( order ) ;
			} // else if
			
			pnode -> height = max( Height( pnode -> left ), Height( pnode -> right ) ) + 1 ;
			return pnode ;
		} // insert()
		
	public :
		AVL() {
			root = NULL ; 
		} // AVL()
		
		int Height() {
			return Height( root ) ;
		} // height()
		
		int Height( nodeType *curN ) {
			if ( curN != NULL )
				return curN -> height ;
			else
				return -1 ;
		} // height
		
		void Mission2( vector<schoolType> input ) {
			nodeType *newNode ;
			insert( root, input.at(0).order, input.at(0).ngrad ) ;
			nodeType *walk = root ;
			for ( int i = 1 ; i < input.size() ; i++ ) {
				insert( walk, input.at(i).order, input.at(i).ngrad ) ;
			} // for
			
			Inorder( walk ) ;
		//	cout << root->graduated << endl ;
			cout << "Tree height =" << Height() << endl ;
		/*	
			for ( int i = 0 ; i < root -> order.size() ; i++ )
				cout << root -> order.at(i) << endl ;
		*/			
		} // Mission2()		
		
		void Inorder( nodeType *curN ) {
			if ( curN == NULL ) {
				return ;
			} // if
			else {
				cout << curN->graduated << endl ;
				Inorder( curN -> left ) ;
				Inorder( curN -> right ) ;
			} // else
			
		} // Inorder()
		
} ; // AVL


int main() {
	SchoolList a ;

	string command ;
	
	while ( true ) {
		cout << "*** Search Tree Utilities ***" << endl ;
		cout << "* 0. QUIT                   *" << endl ;
		cout << "* 1. Build 2-3 tree         *" << endl ;
		cout << "* 2. Build AVL tree         *" << endl ;
		cout << "*****************************" << endl ;
		cout << "Input a choice(0, 1, 2):" ;
		cin >> command ;
		
		if ( command == "0" )
			break ;
		else if ( command == "1" ) {
			if ( a.readFile() ) {
				// a.PrintAllR() ;
				TwoThreeTree m1 ;
				m1.BuildTree( a.GetData() ) ;
			//	m1.Run() ;
				
			} // if
		} // else if
		else if ( command == "2" ) {
			if ( a.readFile() ) {
			//	a.PrintAllR() ;
				AVL tree ;
				tree.Mission2( a.GetData() ) ;
			} // if	
		} // else if
		else
			cout << "Invalid input!" << endl ;
	} // while
	
	return 0 ;	
} // main
