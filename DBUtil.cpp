#include "DBUtil.h"

sqlite3 *pDB = NULL;//���ݿ�ָ��
char * errMsg = NULL;//������Ϣ
std::string sqlstr;//SQLָ��
int result;//sqlite3_exec����ֵ


//�������ݿ�
void DBUtil::initDB(const char *db )
{
    //��һ�����ݿ⣬��������ݿⲻ���ڣ��򴴽�һ�����ݿ��ļ�
    result = sqlite3_open(db, &pDB);
    if( result != SQLITE_OK )
        CCLOG( "�����ݿ�ʧ�ܣ�������:%d ������ԭ��:%s\n" , result, errMsg );
	CCLOG(">db opened");
}


//tableIsExist�Ļص�����
int isExisted( void * para, int n_column, char ** column_value, char ** column_name )
{
    bool *isExisted_=(bool*)para;
    *isExisted_=(**column_value)!='0';
    return 0;
}



//�жϱ���Ƿ����
bool DBUtil::tableIsExist( string name )
{
    if (pDB!=NULL)
    {
        //�жϱ��Ƿ����
        bool tableIsExisted;
        sqlstr = "select count(type) from sqlite_master where type='table' and name ='"+name+"'";
        result =sqlite3_exec(pDB,sqlstr.c_str(),isExisted,&tableIsExisted,&errMsg);
        return tableIsExisted;
    }
    return false;
}



//�����ݿ����ж���Ϊname�ı�ʾ����ڣ�����������򴴽����ű�
//@ʾ�����string sqls = "create table user(id integer,username text,password text)";
void DBUtil::createTable( string sql,string name )
{
    if (!tableIsExist(name))
    {
        //����������IDΪ���������Զ�����
        result = sqlite3_exec(pDB,sql.c_str(),NULL,NULL,&errMsg);
        if( result != SQLITE_OK )
            CCLOG( "������ʧ�ܣ�������:%d ������ԭ��:%s\n" , result, errMsg );
    }
    
}


//ɾ�����
//@ʾ�����sqlstr="drop table name";
void DBUtil::deleteTable( string sql,string name )
{
    if (tableIsExist(name))
    {
        result = sqlite3_exec(pDB,sql.c_str(),NULL,NULL,&errMsg);
        if( result != SQLITE_OK )
            CCLOG( "������ʧ�ܣ�������:%d ������ԭ��:%s\n" , result, errMsg );
    }
}


//��������
//@ʾ�����sqlstr=" insert into MyTable_1( name ) values ( '������' ) ";
void DBUtil::insertData( string sql ){
    result = sqlite3_exec( pDB, sql.c_str() , NULL, NULL, &errMsg );
    if(result != SQLITE_OK )
        CCLOG( "�����¼ʧ�ܣ�������:%d ������ԭ��:%s\n" , result, errMsg );
}


//ɾ������
//@ʾ�����sqlstr="delete from MyTable_1 where ID = 2";
void DBUtil::deleteData( string sql )
{
    result=sqlite3_exec( pDB, sql.c_str() , NULL, NULL, &errMsg );
    if(result != SQLITE_OK )
        CCLOG( "�����¼ʧ�ܣ�������:%d ������ԭ��:%s\n" , result, errMsg );
}


//�޸�����
//@ʾ�����        sqlstr="update MyTable_1 set name='������' where ID = 3";
void DBUtil::updateData( string sql )
{
    result = sqlite3_exec( pDB, sql.c_str() , NULL, NULL, &errMsg );
    if(result != SQLITE_OK )
        CCLOG( "�����¼ʧ�ܣ�������:%d ������ԭ��:%s\n" , result, errMsg );
}


//getDataCount�Ļص�����
int loadRecordCount( void * para, int n_column, char ** column_value, char ** column_name )
{
    int *count=(int*)para;
    *count=n_column;
    return 0;
}
//��ȡ��¼������
//@ʾ�����string sqlsssss = "select count(*) from user";
//@ʾ�����  ȡ�ñ���ֶε����string sqlsssss = "select * from user";
int DBUtil::getDataCount( string sql )
{
    int count=0;
    sqlite3_exec( pDB, sql.c_str() , loadRecordCount, &count, &errMsg );
    return count;
}

vector<map<string,string>> data;


//getDataInfo�Ļص�����
static int loadRecord( void * para, int n_column, char ** column_value, char ** column_name )
{
	map<string,string> tmp;
	for(int i = 0;i<n_column;i++){
		tmp[column_name[i]] = column_value[i];
		//CCLOG("query out:%s,%s.",column_name[i],column_value[i]);
		
	}
	data.push_back(tmp);
    return 0;
}


vector<map<string,string>> DBUtil::getDataInfo( string sql,void *pSend )
{
	data.clear();
    result = sqlite3_exec( pDB, sql.c_str() , loadRecord, pSend, &errMsg );
	return data;

}


//�ر����ݿ�
void DBUtil::closeDB()
{
	CCLOG(">db closed...");
    sqlite3_close(pDB);
}
