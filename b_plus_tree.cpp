#include <iostream>
#include <cstdlib>
#include <ctime>
#include <queue>
using namespace std;
class node
{
public:
	node(int n);
	int level;//阶数
	node **point;//指针，初始为空
	int *index;//索引数值(数据),初始为0
	int position;//当前索引个数,初始为0
	node *right;//b+树的特点，向右指针
};
class b_plus_tree
{
public:
	b_plus_tree(int n);
	int level;//定义b+树的阶数
	node *root;
	void insert(int data);
	node **find_position(int data,node *target,node *father);//寻找要插入的位置，返回比data小的上一个node的位置，返回值[0]:target,[1]:father
	void split_node(node*& target,node*&father,int data);//将target结点分裂
	void insert_sort(int data,int *&index,int number);//对data进行插入排序
	int is_exist(int data,int *&index,int number);//判断是否已经存在
	node *find_father(int data,node *target,node *father);//用data找到结点的父亲结点
	void run_down();//横向遍历
	void split_up_node(node *target,int data);//分裂除叶结点外的结点
	void overTree();//层次遍历整棵树，用于debug
	void delete_data(int data);//删除对应key的数据
	void delete_index(int*& index,int position,int &number);//删除数组中的position下标的元素
	node *get_pre_node(node *target,node *father);//返回target叶子节点的前向节点
	void sort_index(int *&index,int number,int position);//对除position下标外的其余有序数组进行插入排序
	void together_index(int*& pre,int *&bac,int &number_pre,int &number_bac);//将bac上的数据合并至pre
	void delete_father_node(node *&target,int position);//删除索引节点的数据
	int find_father_position(node *pre,node *target,node *father);//找到father节点中左pre右target的index下标
	void move_point(node *target,int position);//将target的position下标后的指针前移,position为index要删除的元素下标
};
int main()
{
	b_plus_tree tree(5);
	tree.insert(1);
	tree.delete_data(1);
	tree.run_down();
	tree.overTree();
}
node::node(int n)
{
	level = n;
	index = new int[level-1]();
	point = new node*[level];
	for(int m=0;m<n;m++)
		point[m] = NULL;
	position = 0;
	right = NULL;
}
b_plus_tree::b_plus_tree(int n)
{
	level = n;
	root = NULL;
}
node **b_plus_tree::find_position(int data,node *target,node *father)
{
	node **target_father = new node*[2];
	target_father[0] = target;
	target_father[1] = father;
	int m;
	for(m=0;m<target->position;m++)
	{
		if(data<target->index[m])
			break;
	}
	if(target->point[m] != NULL)
	{
		delete target_father;
		return find_position(data,target->point[m],target);
	}
	else
	{
		return target_father;
	}
}
void b_plus_tree::split_node(node*& target,node*&father,int data)
{
	int last_data = data;//多出的那个数
	node *up=father,*right;
	if(target->index[target->position-1] > data)//进行排序，将最大值提出至last_data
	{
		last_data = target->index[target->position-1];
		insert_sort(data,target->index,level-2);
	}
	right = new node(level);
	right->right = target->right;//横向连入结点
	target->right = right;
	if(!up)
		up = new node(level);//说明为根结点分裂
	
	int mid = level/2;
	if(up->position == level-1)//处理非叶结点分裂
	{
		split_up_node(up,target->index[mid]);
		node *up_right = up->right;
		node *temp = up->point[0];
		for(int m=0;m<=level/2;m++)//处理叶子节点指针
		{
			up->point[m] = temp;
			temp = temp->right;
		}
		for(int m=0;m<=level/2;m++)
		{
			up_right->point[m] = temp;
			temp = temp->right;
		}

	}
	else
	{
		insert_sort(target->index[mid],up->index,up->position++);//向up中插入数据并排序
		if(up->position == 1)//树长高
		{
			up->point[0] = target;
			up->point[1] = right;
			root = up;
		}
		else
		{
			int i=0;
			node *temp=up->point[0];
			for(int m=0;m<=up->position;m++)
			{
				up->point[i] = temp;
				i++;
				temp = temp->right;
			}

		}//上述代码功能：将父结点指向子结点的指针重置
	}
	for(int m=0;m<level-mid-1;m++)//将target后半部分转至right
	{
		right->index[right->position++] = target->index[mid+m];
		target->index[mid+m] = 0;//还原为默认值0
	}
	right->index[right->position++] = last_data;
	target->position = level/2;//维护position
}
void b_plus_tree::insert(int data)
{
	if(!root)
	{
		root = new node(level);
		int position = root->position;
		root->index[position] = data;
		root->position++;
		return;
	}
	node **target_father = find_position(data,root,NULL);
	node *target = target_father[0];
	node *father = target_father[1];
	int exist = is_exist(data,target->index,target->position);
	if(exist != -1)
	{
		//插入值相同，若要操作在此处进行即可
		return;
	}
	if(target -> position != level-1)//未满,直接插入即可
	{
		insert_sort(data,target->index,target->position);
		target->position++;
		return;
	}
	else//已满，需要分裂
	{
		split_node(target,father,data);
	}
}
void b_plus_tree::insert_sort(int data,int *&index,int number)
{
	int m;
	for(m=number-1;m>=0;m--)
	{
		if(data < index[m])
		{
			index[m+1] = index[m];
		}
		else
			break;
	}
	index[m+1] = data;
}
int b_plus_tree::is_exist(int data,int *&index,int number)
{
	for(int m=0;m<number;m++)
	{
		if(index[m] == data)
			return m;
	}
	return -1;
}
void b_plus_tree::run_down()
{
	if(!root)
		return;
	node *temp = root;
	for(;temp->point[0];temp=temp->point[0]);//寻找叶结点始端
	for(;temp;temp=temp->right)
	{
		for(int m=0;m<temp->position;m++)
		{
			cout<<temp->index[m]<<" ";
		}
	}
}
node *b_plus_tree::find_father(int data,node *target,node *father)
{
	int m;
	for(m=0;m<target->position;m++)
	{
		if(data<=target->index[m])
			break;
	}
	if(data == target->index[m])
		return father;
	else
	{
		return find_father(data,target->point[m],target);
	}
	
}
void b_plus_tree::split_up_node(node *target,int data)
{
	int last_data = data;//多出的那个数
	node *father = find_father(target->index[0],root,NULL);
	node *up=father,*right;
	if(target->index[target->position-1] > data)//进行排序，将最大值提出至last_data
	{
		last_data = target->index[target->position-1];
		insert_sort(data,target->index,level-2);
	}
	right = new node(level);
	right->right = target->right;
	target->right = right;
	if(!up)
		up = new node(level);//说明为根结点分裂

	int mid = level/2;
	if(up->position == level-1)
	{
		split_up_node(up,target->index[mid]);
		node *up_right = up->right;
		int i = 0;
		node *temp = up->point[0];
		for(int m=0;m<=level/2;m++)//处理叶子节点指针
		{
			up->point[m] = temp;
			temp = temp->right;
		}
		for(int m=0;m<=level/2;m++)
		{
			up_right->point[m] = temp;
			temp = temp->right;
		}
	}
	else
	{
	insert_sort(target->index[mid],up->index,up->position++);//向up中插入数据并排序
	if(up->position == 1)//树长高
	{
		up->point[0] = target;
		up->point[1] = right;
		root = up;
	}
	else
	{
		int i=0;
		node *temp=up->point[0];
		for(int m=0;m<=up->position;m++)
		{
			up->point[i] = temp;
			i++;
			temp = temp->right;
		}
	}
	}
	target->index[mid] = 0;
	for(int m=mid+1;m<level-1;m++)
	{
		right->index[right->position++] = target->index[m];
		target->index[m] = 0;
	}
	right->index[right->position++] = last_data;
	target->position = level/2;
}
void b_plus_tree::overTree()
{
	node *temp;
	queue<node*> store_node;
	store_node.push(root);
	while(store_node.size())
	{
		temp = store_node.front();
		int m;
		for(m=0;m<temp->position;m++)
		{
			cout<<temp->index[m]<<" ";
			if(temp->point[m])
			{
				store_node.push(temp->point[m]);
			}
		}
		if(temp->point[m])
		{
			store_node.push(temp->point[m]);
		}
		cout<<endl;
		store_node.pop();
	}
}
void b_plus_tree::delete_data(int data)
{
	node **target_father = find_position(data,root,NULL);
	node *target = target_father[0];
	node *father = target_father[1];
	int m;
	for(m=0;m<target->position;m++)
	{
		if(target->index[m] == data)
			break;
	}
	if(target == root && father == NULL)
	{
		delete_index(target->index,m,target->position);
		return;
	}
	int limit = level/2;
	if(target->position > limit)
	{
		delete_index(target->index,m,target->position);
		return;
	}
	node *pre = get_pre_node(target,father);
	node *brother = pre;
	if(pre && pre->position>limit || !pre && target->right->position>limit)
	{//兄弟节点数目足够
		if(!pre)
		{
			//无前置节点
			brother = target->right;//以下一节点为目标
			int borrow = brother->index[0];
			delete_index(brother->index,0,brother->position);
			target->index[m] = borrow;
			sort_index(target->index,target->position,m);
			int father_position = find_father_position(target,brother,father);
			father->index[father_position] = brother->index[0];
		}
		else
		{
			int borrow = pre->index[pre->position-1];
			pre->index[pre->position-1] = 0;
			pre->position--;
			target->index[m] = borrow;
			sort_index(target->index,target->position,m);
			int father_position = find_father_position(pre,target,father);
			father->index[father_position] = borrow;
		}
	}
	else
	{
		//合并兄弟节点
		delete_index(target->index,m,target->position);
		if(!pre)
		{
			together_index(target->index,target->right->index,target->position,target->right->position);
			node *temp = target->right;
			target->right = temp->right;
			delete temp;
		}
		else
		{
			together_index(pre->index,target->index,pre->position,target->position);
			pre->right = target->right;
			delete target;
		}
		int delete_father_position = 0;
		if(pre)
		{
			for(delete_father_position=0;delete_father_position<father->position;delete_father_position++)
			{
				if(father->point[delete_father_position]==pre && father->point[delete_father_position+1]==target)
					break;
			}
		}
		delete_father_node(father,delete_father_position);
	}

}
void b_plus_tree::delete_index(int*& index,int position,int &number)
{
	int m;
	for(m=position;m<number-1;m++)
	{
		index[m] = index[m+1];
	}
	index[m] = 0;
	number--;
}
node* b_plus_tree::get_pre_node(node*target,node *father)
{
	node *temp = father->point[0];
	node *pre = NULL;
	for(;temp;temp=temp->right)
	{
		if(temp == target)
			return pre;
		pre = temp;
	}
}
void b_plus_tree::sort_index(int *&index,int number,int position)
{
	int data = index[position];
	if(position-1>=0 && data<index[position-1])
	{
		int m;
		for(m=position-1;m>=0;m--)
		{
			if(index[m]<=data)
				break;
			else
			{
				index[m+1] = index[m];
			}
		}
		index[m+1] = data;
	}
	else
	{
		int m;
		for(m=position+1;m<number;m++)
		{
			if(index[m]>=data)
				break;
			else
			{
				index[m-1] = index[m];
			}
		}
		index[m-1] = data;
	}
}
void b_plus_tree::together_index(int*& pre,int *&bac,int &number_pre,int &number_bac)
{
	for(int m=0;m<number_bac;m++)
	{
		pre[number_pre+m] = bac[m];
	}
	number_pre += number_bac;
	number_bac = 0;
}
void b_plus_tree::delete_father_node(node *&target,int position)
{
	if(target == root && target->position==1)
	{
		root = target->point[0];
		delete target;
		target = NULL;
		return;
	}
	else if(target == root)
	{
		delete_index(target->index,position,target->position);
		move_point(target,position);
		return;
	}
	if(target->position>level/2)
	{
		//元素数足够，删除元素后不需要合并
		delete_index(target->index,position,target->position);//删除元素
		move_point(target,position);
	}
	else
	{
		node *father = find_father(target->index[0],root,NULL);
		node *pre = get_pre_node(target,father);
		node *right = target->right;
		if(pre)
		{
			int father_position = find_father_position(pre,target,father);
			//存在pre节点
			if(pre->position>level/2)
			{
				//pre节点富裕,转移兄弟节点
				target->index[position] = father->index[father_position];
				sort_index(target->index,target->position,position);
				father->index[father_position] = pre->index[pre->position-1];
				pre->index[pre->position-1] = 0;
				pre->position--;
				/*node *temp = pre->point[pre->position+1];//处理target的指针
				for(int m=0;m<=target->position;m++)
				{
					target->point[m] = temp;
					temp = temp->right;
				}*/
				//position+1=position ... 1=0
				for(int m=position;m>=0;m--)
					target->point[m+1] = target->point[m];
				target->point[0] = pre->point[pre->position+1];
				pre->point[pre->position+1] = NULL;
			}
			else
			{
				//pre节点不富裕，需要合并
				delete_index(target->index,position,target->position);
				int father_index = father->index[father_position];
				delete_father_node(father,father_position);
				pre->right = target->right;
				//move_point(father,father_position);//移动father的指针
				move_point(target,position);
				for(int m=0;m<=target->position;m++)//移动pre指针
				{
					pre->point[pre->position+1+m] = target->point[m];
				}
				pre->index[pre->position++] = father_index;
				for(int m=0;m<target->position;m++)//将target的值复制到pre中
				{
					pre->index[pre->position++] = target->index[m];
					target->index[m] = 0;
				}
				target->position = 0;
				delete target;
			}
		}
		else
		{
			//不存在pre节点，需要用right节点代替
			int father_position = find_father_position(target,right,father);
			if(right->position>level/2)
			{
				//right节点富裕,转移兄弟节点
				target->index[position] = father->index[father_position];
				sort_index(target->index,target->position,position);
				father->index[father_position] = right->index[0];
				for(int m=position+1;m<target->position;m++)
				{
					target->point[m] = target->point[m+1];
				}
				target->point[target->position] = right->point[0];
				delete_index(right->index,0,right->position);
				/*move_point(target,position);*/
				move_point(right,-1);//right指针前移
			}
			else
			{
				//right节点不富裕，需要合并
				delete_index(target->index,position,target->position);
				int father_index = father->index[father_position];
				delete_father_node(father,father_position);
				target->right = right->right;
				//move_point(father,father_position);//移动father的指针
				move_point(target,position);
				for(int m=0;m<=right->position;m++)//移动target的指针
				{
					target->point[target->position+1+m] = right->point[m];
				}
				target->index[target->position++] = father_index;
				for(int m=0;m<right->position;m++)//将target的值复制到pre中
				{
					target->index[target->position++] = right->index[m];
					right->index[m] = 0;
				}
				right->position = 0;
				delete right;
			}
		}
	}
}
int b_plus_tree::find_father_position(node *pre,node *target,node *father)
{
	int position;
	for(position=0;position<father->position;position++)
	{
		if(father->point[position]==pre && father->point[position+1]==target)
			break;
	}
	return position;
}	
void b_plus_tree::move_point(node *target,int position)
{
	if(!target)
		return;
	int m;
	for(m=position+1;m<=target->position;m++)
	{
		target->point[m] = target->point[m+1];
	}
	target->point[m] = NULL;
}
