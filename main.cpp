#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cassert>
#include <sstream>
#include <algorithm>
#include <cstdlib>

using namespace std;


/*
1 -- 标识符
2 -- 常数
3 -- 保留字
4 -- 运算符
5 -- 界符
*/

/*
Example:

Input:
if(i >= 15) x = y;

Output:
(3, if)
(5, ()
(1, 0) //i在符号表中的入口为0
(4, >=)
(2, 15)
(5, ))
(1, 1) //x的入口为1
(4, =)
(1, 2) //y的入口为2
(5, ;)
*/

const int keywords_num = 23;
const int states_num = 38;
const int comm_state_num = 3;

ifstream file;  //文件流
ofstream outputfile;
char ch; //读取字符


//字母表
char letter[64] = { 'a','b','c','d','e','f','g','h','i','j','k','l','m',
				'n','o','p','q','r','s','t','u','v','w','x','y','z',
				'A','B','C','D','E','F','G','H','I','J','K','L','M',
				'N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};

//数字表
char digit[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

//将类别映射成字符串
string code_to_string[5] = {"标识符", "常量", "关键字", "操作符", "界符"};

//关键字表
string keywords[32] = {"auto", "break", "case", "char", "continue", "do", "default", "double", "else", 
                        "float", "for", "if", "int", "long", "return", "static", "switch", 
                        "struct", "typedef", "void", "unsigned", "while"};

//vector<string> keywordsV(keywords, keywords+20);

//标识符表
vector<string> identifiers;

//操作符表



//状态表
string states[40] = {"start", "comment", "inID", "ID-keywords", "inNum", "Real", "Dec",
                    "number", "LNE", "<", "<>", "<=", "EA", "=", "==", "GE", ">", ">=",
                     "+", "-", "*", "/", ",", ";", "'", "\"", "(", ")", "[", "]", "{", 
                     "}", "+=", "-=", "*=", "/=", "++", "--"};
int state_sets[32];

/**
 * 状态表的初始化
 */
void initStates() {
    for(int i = 0; i < states_num; ++i) {
        state_sets[i] = i;
    }
}

//注释子状态
int comm_state_sets[5] = {0, 1, 2};

//vector<string> state_sets(states, states+11);
/*
enum type{start=0, inID=1, ID_keywords=2, comment=3, inNum=4, 
                    Real=5, Dec=6, number=7, LNE=8, EA=9, GE=10}state_sets;
*/

//词法体
struct Lexical {
    int code;
    string value;
    Lexical(int code, string value) : code(code), value(value) {}
};


/**
 * 显示词法(lexical)
 */
void showLexical(Lexical lexical) {
    cout<<"<"<<lexical.code<<", "<<lexical.value<<">"<<endl;
}


/**
 * 用字符串的形式展示示词法
 */
void displayLexical(Lexical Lexical) {
    cout<<"<"<<code_to_string[Lexical.code - 1]<<", "<<Lexical.value<<">"<<endl;
}


/**
 * 判断是否为letter
 */
bool isLetter(char ch) {
    if((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')) {
        return true;
    }

    return false;
}


/**
 * 判断是否为digit
 */
bool isDigit(char ch) {
    if(ch >= '0' && ch <= '9') {
        return true;
    }

    return false;
}


/**
 * 判断字符串是否匹配关键字
 */
int searchKeywords(string token) {
    for(int i = 0; i < keywords_num; ++i) {
        if(token == keywords[i]) {
            return 3;
        }
    }
    return 0;
}


/**
 * 判断状态是否在状态集合内
 */
bool isInStateSets(int state) {
    for(int i = 0; i < states_num; ++i) {
        if(state == state_sets[i]) {
            return true;
        } else {
            continue;
        }
    }

    return false;
}


/**
 * 判断是否在注释状态表内
 */
bool isInCommStateSets(int state) {
    for(int i = 0; i < comm_state_num; ++i) {
        if(state == comm_state_sets[i]) {
            return true;
        } else {
            continue;
        }
    }

    return false;
}


/**
 * 判断状态是否在inNUm, Real, Dec内
 */
bool isInNnumberState(int state) {
    if(state == 4 || state == 5 || state == 6) {
        return true;
    }

    return false;
}


/**
 * 判断一个标识符是否在标识符表内, 若在, 返回其下标, 否则返回-1
 */
int findIndexInIDTable(string s) {
    for(int i = 0; i < identifiers.size(); ++i) {
        if(identifiers[i] == s) {
            return i;
        } else {
            continue;
        }
    }

    return -1;
}


/**
 * 识别简单的词法
 */
Lexical recognize() {
    int code;
    string value;
    int state = 0;      //初始状态

    string token;

    while(isInStateSets(state)) {
        if(file.eof())
            break;
        
        switch(state) {
            case 1: {       //comment
                int comm_state = 0;
                while(comm_state == 0 || comm_state == 1) {
                    if(file.eof())
                        break;
                    
                    switch(comm_state) {
                        case 0: {
                            if(ch == '*') {
                                comm_state = 1;
                                file >> ch;
                            } else {
                                file >> ch;
                            }
                        }
                        break;

                        case 1: {
                            if(ch == '/') {
                                comm_state = 2;
                                file >> ch;
                            } else if(ch == '*') {
                                file >> ch;   //状态不变, 还是1状态
                            } else {
                                comm_state = 0;
                                file >> ch;
                            }
                        }
                        break;
                    }
                } //end while

                if(comm_state == 2) {
                    state = 0;
                    file >> ch;
                }
                /*
                if(ch == '}') {
                    state = 0;  //start
                    file >> ch;
                } else {
                    file >> ch;
                }
                */
            }
            break;

            case 0: {       //start
                if(isLetter(ch)) {
                    state = 2; //inID
                    token += ch;
                    file >> ch;
                } else if(isDigit(ch)) {
                    state = 4;  //inNum
                    token += ch;
                    file >> ch;
                } else if(ch == '<') {
                    state = 8;  //LNE
                    token += ch;
                    file >>ch;
                } else if(ch == '=') {
                    state = 12;  //EA
                    token += ch;
                    file >> ch;
                } else if(ch == '>') {
                    state = 15;  //GE
                    token += ch;
                    file >> ch;
                } else if(ch == '+') {
                    state = 18;  //+
                    token += ch;
                    file >> ch;
                } else if(ch == '-') {
                    state = 19;  //-
                    token += ch;
                    file >> ch;
                } else if(ch == '*') {
                    state = 20;  // *
                    token += ch;
                    file >> ch;
                } else if(ch == '/') {
                    state = 21;  // /
                    token += ch;
                    file >> ch;
                } else if(ch == ',') {
                    state = 22;   //,
                    token += ch;
                } else if(ch == ';') {
                    state = 23;   //;
                    token += ch;
                } else if(ch == '\'') {
                    state = 24;   //'
                    token += ch;
                } else if(ch == '"') {
                    state = 25;   //"
                    token += ch;
                } else if(ch == '(') {
                    state = 26;   //(
                    token += ch;
                } else if(ch == ')') {
                    state = 27;   //)
                    token += ch;
                } else if(ch == '[') {
                    state = 28;   //[
                    token += ch;
                } else if(ch == ']') {
                    state = 29;   //]
                    token += ch;
                } else if(ch == '{') {
                    state = 30;
                    token += ch;
                } else if(ch == '}') {
                    state = 31;
                    token += ch;
                } else {
                    file >> ch;     //将没用的字符过滤掉
                }
            }
            break;

            case 2: {       //inID
                while(isLetter(ch) || isDigit(ch)) {
                    if(file.eof())
                        break;
                    token += ch;
                    file >> ch;
                }

                code = searchKeywords(token);     //在关键字表中查找token, 看是否匹配
                if(code != 0) {             
                    return Lexical(3, token);       //返回关键字的词法记号  3->关键字
                } else {
                    int tmp;
                    if(findIndexInIDTable(token) != -1) {
                        tmp = findIndexInIDTable(token);    //记录其在标识符表中的位置
                    } else {
                        identifiers.push_back(token);
                        tmp = identifiers.size() - 1;   //记录其在标识符表中的位置
                    }
                    stringstream ss;
                    ss << tmp;
                    ss >> value;
                    return Lexical(1, value);       //返回标识符的词法记号  1->标识符
                }
            }
            break;

            case 4: {      //inNum
                while(isInNnumberState(state)) {
                    if(file.eof())
                        break;

                    switch(state) {
                        case 4: {       //inNum
                            if(isDigit(ch)) {
                                //cout<<token<<endl;
                                state = 4;      //inNum
                                token += ch;
                                file >> ch;
                            } else if(ch == '.') {
                                //cout<<token<<endl;
                                state = 5;      //Real
                                token += ch;
                                file >> ch;
                            } else {
                                state = 7;  //number
                                code = 2;   //2->常量
                            }
                        }
                        break;

                        case 5: {       //Real
                            if(isDigit(ch)) {
                                //cout<<token<<endl;
                                state = 6;      //Dec
                                token += ch;
                                file >> ch;
                            } else {
                                cout<<"reporterror"<<endl;    //error
                            }
                        }
                        break;

                        case 6: {   //Dec
                            if(isDigit(ch)) {
                                state = 6;  //Dec
                                token += ch;
                                file >> ch;
                            } else {
                                code = 2;   //2->常量
                                state = 7;  //number
                            }
                        }
                        break;
                    }
                } //end while
            }
            break;

            case 7: {
                //file >> ch;
                /*
                identifiers.push_back(token);
                int tmp = identifiers.size() - 1;
                stringstream ss;
                ss << tmp;
                ss >> value;
                */
                return Lexical(code, token);
            }
            break;

            case 8: {   //LNE
                if(ch == '>') {
                    file >> ch;
                    return Lexical(4, "<>");    //4->操作符
                } else if(ch == '=') {
                    file >> ch;
                    return Lexical(4, "<=");
                } else {
                    file >> ch;
                    return Lexical(4, "<");
                }
            }
            break;

            case 12: {  //EA
                if(ch == '=') {
                    file >> ch;
                    return Lexical(4, "==");    //4->操作符
                } else {
                    file >> ch;
                    return Lexical(4, "=");
                }
            }
            break;

            case 15: {  //GE
                if(ch == '=') {
                    file >> ch;
                    return Lexical(4, ">=");   //4->操作符
                } else {
                    file >> ch;
                    return Lexical(4, ">");
                }
            }
            break;

            case 18: {  //+
                if(ch == '=') {
                    file >> ch;
                    return Lexical(4, "+=");
                } else if(ch == '+') {
                    file >> ch;
                    return Lexical(4, "++");
                } else {
                    file >> ch;
                    return Lexical(4, "+");
                }
            }
            break;

            case 19: {  //-
                if(ch == '=') {
                    return Lexical(4, "-=");
                } else if(ch == '-') {
                    file >> ch;
                    return Lexical(4, "--");
                } else {
                    file >> ch;
                    return Lexical(4, "-");
                }
            }
            break;

            case 20: {  //*
                if(ch == '=') {
                    file >> ch;
                    return Lexical(4, "*=");
                } else {
                    file >> ch;
                    return Lexical(4, "*");
                }
            }
            break;

            case 21: {  // /
                if(ch == '*') {
                    state = 1;
                    token += ch;
                    file >> ch;
                } else if(ch == '=') {
                    file >> ch;
                    return Lexical(4, "/=");
                } else {
                    file >>ch;
                    return Lexical(4, "/");
                }
            }
            break;

            case 22: {  //,
                file >>ch;
                return Lexical(5, ",");
            }
            break;

            case 23: {  //;
                file >>ch;
                return Lexical(5, ";");
            }
            break;

            case 24: {  //'
                file >>ch;
                return Lexical(5, "'");
            }
            break;

            case 25: {  //"
                file >>ch;
                return Lexical(5, "\"");
            }
            break;

            case 26: {  //(
                file >>ch;
                return Lexical(5, "(");
            }
            break;

            case 27: {  //)
                file >>ch;
                return Lexical(5, ")");
            }
            break;

            case 28: {  //[
                file >>ch;
                return Lexical(5, "[");
            }
            break;

            case 29: {  //]
                file >>ch;
                return Lexical(5, "]");
            }
            break;

            case 30: {  //{
                file >>ch;
                return Lexical(5, "{");
            }
            break;

            case 31: {  //}
                file >> ch;
                return Lexical(5, "}");
            }
            break;
        }
    } //end while

    return Lexical(code, value);
}


/**
 * 写入文件
 */
void write_to_file(Lexical Lexical) {
    outputfile<<"<"<<code_to_string[Lexical.code - 1]<<", "<<Lexical.value<<">\n";
}


int main() {
    initStates();
    file.open("code.txt");
    outputfile.open("output.txt");
    assert(file.is_open());    //若文件打开失败, 则报错
    assert(outputfile.is_open());
    file >> noskipws;       //读取字符, 不跳过空格与回车
    //char ch;
    file>>ch;
    while(!file.eof()) {
        Lexical lexical = recognize();
        if(lexical.value != "") {
            //showLexical(lexical);
            displayLexical(lexical);
            write_to_file(lexical);
        }
    }
    file.close();
    outputfile.close();

    return 0;
}