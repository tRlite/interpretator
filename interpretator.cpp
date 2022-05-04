#include <iostream>
#include <string>
#include <cstdio>
#include <ctype.h>
#include <cstdlib>
#include <vector>
#include <stack>
#include <algorithm>
#include <cstring>

using namespace std;

enum type_of_lex {
    LEX_NULL,
    LEX_AND, LEX_BOOL, LEX_ELSE, LEX_IF, LEX_FALSE, LEX_INT,
    LEX_NOT, LEX_OR, LEX_PROGRAM, LEX_READ, LEX_TRUE, LEX_DO, LEX_FOR, LEX_WHILE, LEX_WRITE, LEX_STRING,
    LEX_FIN,
    LEX_SEMICOLON, LEX_COMMA, LEX_COLON, LEX_ASSIGN, LEX_LPAREN, LEX_RPAREN, LEX_EQ, LEX_LSS,
    LEX_GTR, LEX_PLUS, LEX_MINUS, LEX_TIMES, LEX_SLASH, LEX_LEQ, LEX_NEQ, LEX_GEQ, LEX_LFIG, LEX_RFIG, LEX_UMINUS,
    LEX_NUM,
    LEX_ID,
    LEX_WORD,
    POLIZ_LABEL,
    POLIZ_ADDRESS,
    POLIZ_GO,
    POLIZ_FGO
};

/////////////////////////  Класс Lex  //////////////////////////

class Lex {
    type_of_lex   t_lex;
    int           v_lex;
public:
    Lex(type_of_lex t = LEX_NULL, int v = 0) : t_lex(t), v_lex(v) { }
    type_of_lex  get_type() const {
        return t_lex;
    }
    int get_value() const {
        return v_lex;
    }
    friend ostream& operator<< (ostream& s, Lex l);
};

/////////////////////  Класс Ident  ////////////////////////////

class Ident {
    string      name;
    bool        declare;
    type_of_lex type;
    bool        assign;
    int          value;
public:
    Ident() {
        declare = false;
        assign = false;
    }
    bool operator== (const string& s) const {
        return name == s;
    }
    Ident(const string n) {
        name = n;
        declare = false;
        assign = false;
    }
    string get_name() const {
        return name;
    }
    bool get_declare() const {
        return declare;
    }
    void put_declare() {
        declare = true;
    }
    type_of_lex get_type() const {
        return type;
    }
    void put_type(type_of_lex t) {
        type = t;
    }
    bool get_assign() const {
        return assign;
    }
    void put_assign() {
        assign = true;
    }
    int  get_value() const {
        return value;
    }
    void put_value(int v) {
        value = v;
    }
};

//////////////////////  TID, TTW  ///////////////////////

vector<Ident> TID;
vector<string> TTW;

int put(const string& buf) {
    vector<Ident>::iterator k;

    if ((k = find(TID.begin(), TID.end(), buf)) != TID.end())
        return k - TID.begin();
    TID.push_back(Ident(buf));
    return TID.size() - 1;
}

int put_ttw(const string& str) {
    vector<string>::iterator k;

    if ((k = find(TTW.begin(), TTW.end(), str)) != TTW.end())
        return k - TTW.begin();
    TTW.push_back(str);
    return TTW.size() - 1;
}
/////////////////////////////////////////////////////////////////

class Scanner {
    FILE* fp;
    char   c;
    int look(const string buf, const char** list) {
        int i = 0;
        while (list[i]) {
            if (buf == list[i])
                return i;
            ++i;
        }
        return 0;
    }
    void gc() {
        c = fgetc(fp);
    }
public:
    static const char* TW[], * TD[];
    Scanner(const char* program) {
        if (!(fp = fopen(program, "r")))
            throw  "can’t open file";
    }
    Lex get_lex();
};

const char*
Scanner::TW[] = { "", "and", "bool", "else", "if", "false", "int", "not", "or", "program",
                      "read", "true", "do", "for", "while", "write","string", NULL };

const char*
Scanner::TD[] = { "@", ";", ",", ":","=", "(", ")","==", "<", ">", "+", "-", "*", "/", "<=", "!=", ">=","{","}", NULL };

Lex Scanner::get_lex() {
    enum    state { H, IDENT, NUMB, STR, COM, COM2, ALE, NEQ };
    int     d, j;
    string  buf, str;
    state   CS = H;
    do {
        gc();
        switch (CS) {
        case H:
            if (c == ' ' || c == '\n' || c == '\r' || c == '\t');
            else if (isalpha(c)) {
                buf.push_back(c);
                CS = IDENT;
            }
            else if (isdigit(c)) {
                d = c - '0';
                CS = NUMB;
            }
            else if (c == '"') {
                CS = STR;
            }
            else if (c == '/') {
                CS = COM;
            }
            else if (c == '=' || c == '<' || c == '>') {
                buf.push_back(c);
                CS = ALE;
            }
            else if (c == '@')
                return Lex(LEX_FIN);
            else if (c == '!') {
                buf.push_back(c);
                CS = NEQ;
            }
            else {
                buf.push_back(c);
                if ((j = look(buf, TD))) {
                    return Lex((type_of_lex)(j + (int)LEX_FIN), j);
                }
                else
                    throw c;
            }
            break;
        case IDENT:
            if (isalpha(c) || isdigit(c)) {
                buf.push_back(c);
            }
            else {
                ungetc(c, fp);
                if ((j = look(buf, TW))) {
                    return Lex((type_of_lex)j, j);
                }
                else {
                    j = put(buf);
                    return Lex(LEX_ID, j);
                }
            }
            break;
        case NUMB:
            if (isdigit(c)) {
                d = d * 10 + (c - '0');
            }
            else {
                ungetc(c, fp);
                return Lex(LEX_NUM, d);
            }
            break;
        case STR:
            if (c != '"')
                str.push_back(c);
            else {
                j = put_ttw(str);
                return Lex(LEX_WORD, j);
            }
            break;
        case COM:
            if (c == '/') {
                CS = COM2;
            }
            else {
                ungetc(c, fp);
                j = look(buf, TD);
                return Lex((type_of_lex)(j + (int)LEX_FIN), j);
            };
            break;
        case COM2:
            if (c == '\n') {
                CS = H;
            }
            else if (c == '@')
                throw c;
            break;
        case ALE:
            if (c == '=') {
                buf.push_back(c);
                j = look(buf, TD);
                return Lex((type_of_lex)(j + (int)LEX_FIN), j);
            }
            else {
                ungetc(c, fp);
                j = look(buf, TD);
                return Lex((type_of_lex)(j + (int)LEX_FIN), j);
            }
            break;
        case NEQ:
            if (c == '=') {
                buf.push_back(c);
                j = look(buf, TD);
                return Lex(LEX_NEQ, j);
            }
            else
                throw '!';
            break;
        } //end switch
    } while (true);
}

ostream& operator<< (ostream& s, Lex l) {
    string t;
    if (l.t_lex <= LEX_STRING)
        t = Scanner::TW[l.t_lex];
    else if (l.t_lex >= LEX_FIN && l.t_lex <= LEX_RFIG)
        t = Scanner::TD[l.t_lex - LEX_FIN];
    else if (l.t_lex == LEX_UMINUS)
        t = "^";
    else if (l.t_lex == LEX_NUM)
        t = "NUMB";
    else if (l.t_lex == LEX_ID)
        t = TID[l.v_lex].get_name();
    else if (l.t_lex == LEX_WORD)
        t = TTW[l.v_lex];
    else if (l.t_lex == POLIZ_LABEL)
        t = "Label";
    else if (l.t_lex == POLIZ_ADDRESS)
        t = "Addr";
    else if (l.t_lex == POLIZ_GO)
        t = "!";
    else if (l.t_lex == POLIZ_FGO)
        t = "!F";
    else
        throw l;
    s << '(' << t << ',' << l.v_lex << ");" << endl;
    return s;
}



//////////////////////////  Класс Parser  /////////////////////////////////
template <class T, class T_EL>
void from_st(T& st, T_EL& i) {
    i = st.top(); st.pop();
}

class Parser {
    Lex          curr_lex;
    type_of_lex  c_type;
    int          c_val;
    Scanner      scan;
    stack < type_of_lex >   st_lex;
    void  P();
    void  D1();
    void  D(string type_of_variable);
    void  B();
    void  S();
    void  E();
    void  E1();
    void  T();
    void  F();
    void  dec(type_of_lex type, int i);
    void  check_id();
    void  check_op();
    void  check_not();
    void  eq_type();
    void  eq_bool();
    void  check_id_in_read();
    void  check_uminus();
    void  gl() {
        curr_lex = scan.get_lex();
        c_type = curr_lex.get_type();
        c_val = curr_lex.get_value();
    }
public:
    vector <Lex> poliz;
    Parser(const char* program) : scan(program) { }
    void  analyze();
};

void Parser::analyze() {
    gl();
    P();
    if (c_type != LEX_FIN)
        throw curr_lex;
    //for_each( poliz.begin(), poliz.end(), [](Lex l){ cout << l; });
    for (Lex l : poliz)
        cout << l;
    cout << endl << "Yes!!!" << endl;
}

void Parser::P() {
    if (c_type == LEX_PROGRAM) {
        gl();
    }
    else
        throw curr_lex;

    if (c_type == LEX_LFIG) {
        gl();
    }
    else throw curr_lex;

    while (c_type == LEX_INT or c_type == LEX_BOOL or c_type == LEX_STRING) {
        D1();
        if (c_type == LEX_SEMICOLON)
            gl();
        else
            throw curr_lex;
    }
    B();
    if (c_type == LEX_RFIG)
        gl();
    else
        throw curr_lex;
}

void Parser::D1() {
    if (c_type == LEX_INT) {
        gl();
        D("int");
    }
    else if (c_type == LEX_BOOL) {
        gl();
        D("bool");
    }
    else if (c_type == LEX_STRING) {
        gl();
        D("string");
    }
    else
        throw curr_lex;
}

void Parser::D(string type_of_variable) {
    if (c_type != LEX_ID)
        throw curr_lex;
    else {
        int c = c_val;
        if (type_of_variable == "int") {
            dec(LEX_INT, c_val);
        }
        else
            if (type_of_variable == "bool") {
                dec(LEX_BOOL, c_val);
            }
            else if (type_of_variable == "string") {
                dec(LEX_STRING, c_val);
            }
            else
                throw curr_lex;
        gl();
        if (c_type == LEX_ASSIGN) {
            poliz.push_back(Lex(POLIZ_ADDRESS, c));
            st_lex.push(TID[c].get_type());
            gl();
            E();
            eq_type();
            poliz.push_back(Lex(LEX_ASSIGN));
        }
        while (c_type == LEX_COMMA) {
            gl();
            c = c_val;
            if (c_type != LEX_ID)
                throw curr_lex;
            else {
                if (type_of_variable == "int") {
                    dec(LEX_INT, c_val);
                }
                else
                    if (type_of_variable == "bool") {
                        dec(LEX_BOOL, c_val);
                    }
                    else if (type_of_variable == "string") {
                        dec(LEX_STRING, c_val);
                    }
                    else
                        throw curr_lex;
                gl();
                if (c_type == LEX_ASSIGN) {
                    poliz.push_back(Lex(POLIZ_ADDRESS, c));
                    st_lex.push(TID[c].get_type());
                    gl();
                    E();
                    eq_type();
                    poliz.push_back(Lex(LEX_ASSIGN));
                }
            }
        }
    }
}

void Parser::B() {
    S();
    while (c_type == LEX_SEMICOLON) {
        gl();
        S();
    }
}

void Parser::S() {
    int pl0, pl1, pl2, pl3;

    if (c_type == LEX_IF) {
        gl();
        if (c_type == LEX_LPAREN) {
            gl();
            E();
            eq_bool();
            if (c_type == LEX_RPAREN) {
                gl();
            }
            else throw curr_lex;
        }
        else throw curr_lex;

        pl2 = poliz.size();
        poliz.push_back(Lex());
        poliz.push_back(Lex(POLIZ_FGO));
        if (c_type == LEX_LFIG) {
            gl();
            while (c_type != LEX_RFIG) {
                S();
                if (c_type == LEX_SEMICOLON)
                    gl();
                else throw curr_lex;
                if (c_type == LEX_FIN) {
                    cout << "Check the correct placment of {}" << endl;
                    throw curr_lex;
                }
            }
            pl3 = poliz.size();
            poliz.push_back(Lex());
            poliz.push_back(Lex(POLIZ_GO));
            poliz[pl2] = Lex(POLIZ_LABEL, poliz.size());
            gl();
            if (c_type == LEX_ELSE) {
                gl();
                if (c_type == LEX_LFIG) {
                    gl();
                    while (c_type != LEX_RFIG) {
                        S();
                        if (c_type == LEX_SEMICOLON)
                            gl();
                        else throw curr_lex;
                        if (c_type == LEX_FIN) {
                            cout << "Check the correct placment of {}" << endl;
                            throw curr_lex;
                        }
                    }
                    gl();
                }
                else throw curr_lex;
            }
            poliz[pl3] = Lex(POLIZ_LABEL, poliz.size());
        }
        else
            throw curr_lex;
    }//end if
    else if (c_type == LEX_WHILE) {
        pl0 = poliz.size();
        gl();
        if (c_type == LEX_LPAREN) {
            gl();
            E();
            eq_bool();
            if (c_type == LEX_RPAREN) gl();
            else throw curr_lex;
        }
        else throw curr_lex;

        pl1 = poliz.size();
        poliz.push_back(Lex());
        poliz.push_back(Lex(POLIZ_FGO));
        if (c_type == LEX_LFIG) {
            gl();
            while (c_type != LEX_RFIG) {
                S();
                if (c_type == LEX_SEMICOLON)
                    gl();
                else throw curr_lex;
                if (c_type == LEX_FIN) {
                    cout << "Check the correct placment of {}" << endl;
                    throw curr_lex;
                }
            }
            poliz.push_back(Lex(POLIZ_LABEL, pl0));
            poliz.push_back(Lex(POLIZ_GO));
            poliz[pl1] = Lex(POLIZ_LABEL, poliz.size());
            gl();
        }
        else
            throw curr_lex;

    }//end while
    else if (c_type == LEX_DO) {
        pl0 = poliz.size();
        gl();
        if (c_type == LEX_LFIG) {
            gl();
            while (c_type != LEX_RFIG) {
                S();
                if (c_type == LEX_SEMICOLON)
                    gl();
                else throw curr_lex;
                if (c_type == LEX_FIN) {
                    cout << "Check the correct placment of {}" << endl;
                    throw curr_lex;
                }
            }
            gl();
            if (c_type == LEX_WHILE) {
                gl();
                if (c_type == LEX_LPAREN) {
                    gl();
                    E();
                    eq_bool();
                    if (c_type == LEX_RPAREN) gl();
                    else throw curr_lex;
                }
                else throw curr_lex;

            }
            else throw curr_lex;
            pl1 = poliz.size();
            poliz.push_back(Lex());
            poliz.push_back(Lex(POLIZ_FGO));
            poliz.push_back(Lex(POLIZ_LABEL, pl0));
            poliz.push_back(Lex(POLIZ_GO));
            poliz[pl1] = Lex(POLIZ_LABEL, poliz.size());
        }
        else throw curr_lex;
    }//end do-while 
    else if (c_type == LEX_FOR) {
        gl();
        if (c_type == LEX_LPAREN) {
            gl();
            E();
            if (c_type == LEX_SEMICOLON) gl();
            else throw curr_lex;
            pl0 = poliz.size();
            E();
            eq_bool();
            pl1 = poliz.size();
            poliz.push_back(Lex());
            poliz.push_back(Lex(POLIZ_FGO));
            pl2 = poliz.size();
            poliz.push_back(Lex());
            poliz.push_back(Lex(POLIZ_GO));
            if (c_type == LEX_SEMICOLON) gl();
            else throw curr_lex;
            pl3 = poliz.size();
            E();
            poliz.push_back(Lex(POLIZ_LABEL, pl0));
            poliz.push_back(Lex(POLIZ_GO));
            if (c_type == LEX_RPAREN) gl();
            else throw curr_lex;
            if (c_type == LEX_LFIG) {
                gl();
                while (c_type != LEX_RFIG) {
                    poliz[pl2] = Lex(POLIZ_LABEL, poliz.size());
                    S();
                    if (c_type == LEX_SEMICOLON)
                        gl();
                    else throw curr_lex;
                    if (c_type == LEX_FIN) {
                        cout << "Check the correct placment of {}" << endl;
                        throw curr_lex;
                    }
                }
                gl();
            }
            else throw curr_lex;
            poliz.push_back(Lex(POLIZ_LABEL, pl3));
            poliz.push_back(Lex(POLIZ_GO));
            poliz[pl1] = Lex(POLIZ_LABEL, poliz.size());
        }
        else throw curr_lex;
    }//end for 
    else if (c_type == LEX_READ) {
        gl();
        if (c_type == LEX_LPAREN) {

            do {
                gl();
                if (c_type == LEX_ID) {
                    check_id_in_read();
                    poliz.push_back(Lex(POLIZ_ADDRESS, c_val));
                    gl();
                    poliz.push_back(Lex(LEX_READ));
                }
                else
                    throw curr_lex;
            } while (c_type == LEX_COMMA);
            if (c_type == LEX_RPAREN) {
                gl();
            }
            else
                throw curr_lex;
        }
        else
            throw curr_lex;
    }//end read
    else if (c_type == LEX_WRITE) {
        gl();
        if (c_type == LEX_LPAREN) {
            do {
                gl();
                E();
                poliz.push_back(Lex(LEX_WRITE));
            } while (c_type == LEX_COMMA);
            if (c_type == LEX_RPAREN) {
                gl();
            }
            else
                throw curr_lex;
        }
        else
            throw curr_lex;
    }//end write
    else if (c_type == LEX_ID) {
        check_id();
        poliz.push_back(Lex(POLIZ_ADDRESS, c_val));
        gl();
        if (c_type == LEX_ASSIGN) {
            gl();
            E();
            eq_type();
            poliz.push_back(Lex(LEX_ASSIGN));
        }
        else
            throw curr_lex;
    }//assign-end
}

void Parser::E() {
    E1();
    if (c_type == LEX_EQ || c_type == LEX_LSS || c_type == LEX_GTR ||
        c_type == LEX_LEQ || c_type == LEX_GEQ || c_type == LEX_NEQ) {
        st_lex.push(c_type);
        gl();
        E1();
        check_op();
    }
}

void Parser::E1() {
    T();
    while (c_type == LEX_PLUS || c_type == LEX_MINUS || c_type == LEX_OR) {
        st_lex.push(c_type);
        gl();
        T();
        check_op();
    }
}

void Parser::T() {
    F();
    while (c_type == LEX_TIMES || c_type == LEX_SLASH || c_type == LEX_AND) {
        st_lex.push(c_type);
        gl();
        F();
        check_op();
    }
}

void Parser::F() {
    bool uminus = false;
    if (c_type == LEX_MINUS) {
        uminus = true;
        gl();
    }
    if (c_type == LEX_ID) {
        int c = c_val;
        check_id();
        gl();
        if (c_type == LEX_ASSIGN) {
            poliz.push_back(Lex(POLIZ_ADDRESS, c));
            gl();
            E();
            eq_type();
            poliz.push_back(Lex(LEX_ASSIGN));
            st_lex.push(TID[c].get_type());
        }
        else poliz.push_back(Lex(LEX_ID, c));

    }
    else if (c_type == LEX_NUM) {
        st_lex.push(LEX_INT);
        poliz.push_back(curr_lex);
        gl();
    }
    else if (c_type == LEX_TRUE) {
        st_lex.push(LEX_BOOL);
        poliz.push_back(Lex(LEX_TRUE, 1));
        gl();
    }
    else if (c_type == LEX_FALSE) {
        st_lex.push(LEX_BOOL);
        poliz.push_back(Lex(LEX_FALSE, 0));
        gl();
    }
    else if (c_type == LEX_WORD) {
        st_lex.push(LEX_STRING);
        poliz.push_back(curr_lex);
        gl();
    }
    else if (c_type == LEX_NOT) {
        gl();
        F();
        check_not();
    }
    else if (c_type == LEX_LPAREN) {
        gl();
        E();
        if (c_type == LEX_RPAREN)
            gl();
        else
            throw curr_lex;
    }
    else
        throw curr_lex;
    if (uminus)
        check_uminus();
}

////////////////////////////////////////////////////////////////

void Parser::dec(type_of_lex type, int i) {
    if (TID[i].get_declare())
        throw "twice";
    else {
        TID[i].put_declare();
        TID[i].put_type(type);
    }
}


void Parser::check_id() {
    if (TID[c_val].get_declare())
        st_lex.push(TID[c_val].get_type());
    else
        throw "not declared";
}

void Parser::check_op() {
    type_of_lex t1, t2, op, t = LEX_INT, r = LEX_BOOL;

    from_st(st_lex, t2);
    from_st(st_lex, op);
    from_st(st_lex, t1);
    if (t1 == LEX_STRING) {
        t = LEX_STRING;
        if (op == LEX_PLUS)
            r = LEX_STRING;
    }
    if (t1 == LEX_INT && (op == LEX_PLUS || op == LEX_MINUS || op == LEX_TIMES || op == LEX_SLASH))
        r = LEX_INT;
    if (op == LEX_OR || op == LEX_AND)
        t = LEX_BOOL;
    if (t1 == t2 && t1 == t)
        st_lex.push(r);
    else
        if (op == LEX_TIMES && ((t1 == LEX_STRING && t2 == LEX_INT) xor (t2 == LEX_STRING && t1 == LEX_INT)))
            st_lex.push(LEX_STRING);
        else throw "wrong types are in operation";
    poliz.push_back(Lex(op));
}

void Parser::check_not() {
    if (st_lex.top() != LEX_BOOL)
        throw "wrong type is in not";
    else
        poliz.push_back(Lex(LEX_NOT));
}

void Parser::check_uminus() {
    if (st_lex.top() != LEX_INT)
        throw "wrong type is in unary minus";
    else
        poliz.push_back(Lex(LEX_UMINUS));
}

void Parser::eq_type() {
    type_of_lex t;
    from_st(st_lex, t);
    if (t != st_lex.top())
        throw "wrong types are in =";
    st_lex.pop();
}

void Parser::eq_bool() {
    if (st_lex.top() != LEX_BOOL)
        throw "expression is not boolean";
    st_lex.pop();
}

void Parser::check_id_in_read() {
    if (!TID[c_val].get_declare())
        throw "not declared";
}

////////////////////////////////////////////////////////////////

class Executer {
public:
    void execute(vector<Lex>& poliz);
};

void Executer::execute(vector<Lex>& poliz) {
    Lex pc_el;
    stack < int > args;
    stack < string > string_args;
    int i, j, index = 0, size = poliz.size();
    string curr_str, curr_str2;
    while (index < size) {
        pc_el = poliz[index];
        switch (pc_el.get_type()) {
        case LEX_TRUE: case LEX_FALSE: case LEX_NUM: case POLIZ_ADDRESS: case POLIZ_LABEL:
            args.push(pc_el.get_value());
            break;

        case LEX_WORD:
            string_args.push(TTW[pc_el.get_value()]);
            args.push('s');
            break;

        case LEX_ID:
            i = pc_el.get_value();
            if (TID[i].get_assign()) {
                if (TID[i].get_type() == LEX_STRING) {
                    string_args.push(TTW[TID[i].get_value()]);
                    args.push('s');
                }
                else
                    args.push(TID[i].get_value());
                break;
            }
            else
                throw "POLIZ: indefinite identifier";

        case LEX_NOT:
            from_st(args, i);
            args.push(!i);
            break;

        case LEX_UMINUS:
            from_st(args, i);
            args.push(-i);
            break;

        case LEX_OR:
            from_st(args, i);
            from_st(args, j);
            args.push(j || i);
            break;

        case LEX_AND:
            from_st(args, i);
            from_st(args, j);
            args.push(j && i);
            break;

        case POLIZ_GO:
            from_st(args, i);
            index = i - 1;
            break;

        case POLIZ_FGO:
            from_st(args, i);
            from_st(args, j);
            if (!j) index = i - 1;
            break;

        case LEX_WRITE:
            from_st(args, j);
            if (j != 's')
                cout << j << endl;
            else {
                from_st(string_args, curr_str);
                cout << curr_str << endl;
            }
            break;

        case LEX_READ:
            int k;
            from_st(args, i);
            if (TID[i].get_type() == LEX_INT) {
                cout << "Input int value for " << TID[i].get_name() << endl;
                cin >> k;
            }
            else {
                string j;
                if (TID[i].get_type() == LEX_BOOL)
                    while (1) {
                        cout << "Input boolean value (true or false) for " << TID[i].get_name() << endl;
                        cin >> j;
                        if (j != "true" && j != "false") {
                            cout << "Error in input:true/false" << endl;
                            continue;
                        }
                        k = (j == "true") ? 1 : 0;
                        break;
                    }
                else {
                    cout << "Input string value for " << TID[i].get_name() << endl;
                    cin >> j;
                    k = put_ttw(j);
                }
            }
            TID[i].put_value(k);
            TID[i].put_assign();
            break;

        case LEX_PLUS:
            from_st(args, i);
            from_st(args, j);
            if (i != 's')
                args.push(i + j);
            else {
                from_st(string_args, curr_str2);
                from_st(string_args, curr_str);
                string_args.push(curr_str + curr_str2);
                args.push('s');
            }
            break;

        case LEX_TIMES:
            from_st(args, i);
            from_st(args, j);
            if (i != 's' && j != 's')
                args.push(i * j);
            else {
                from_st(string_args, curr_str);
                if (i != 's') {
                    curr_str2 = curr_str;
                    for (int r = 1; r < i; r++) curr_str2.append(curr_str);
                    string_args.push(curr_str2);
                    args.push('s');
                }
                else {
                    curr_str2 = curr_str;
                    for (int r = 1; r < j; r++) curr_str2.append(curr_str);
                    string_args.push(curr_str2);
                    args.push('s');
                }
            }
            break;

        case LEX_MINUS:
            from_st(args, i);
            from_st(args, j);
            args.push(j - i);
            break;

        case LEX_SLASH:
            from_st(args, i);
            from_st(args, j);
            if (!i) {
                args.push(j / i);
                break;
            }
            else
                throw "POLIZ:divide by zero";

        case LEX_EQ:
            from_st(args, i);
            from_st(args, j);
            if (i == 's') {
                from_st(string_args, curr_str2);
                from_st(string_args, curr_str);
                args.push(curr_str == curr_str2);
            }
            else args.push(i == j);
            break;

        case LEX_LSS:
            from_st(args, i);
            from_st(args, j);
            if (i == 's') {
                from_st(string_args, curr_str2);
                from_st(string_args, curr_str);
                args.push(curr_str < curr_str2);
            }
            else args.push(j < i);
            break;

        case LEX_GTR:
            from_st(args, i);
            from_st(args, j);
            if (i == 's') {
                from_st(string_args, curr_str2);
                from_st(string_args, curr_str);
                args.push(curr_str > curr_str2);
            }
            else args.push(j > i);
            break;

        case LEX_LEQ:
            from_st(args, i);
            from_st(args, j);
            if (i == 's') {
                from_st(string_args, curr_str2);
                from_st(string_args, curr_str);
                args.push(curr_str <= curr_str2);
            }
            else args.push(j <= i);
            break;

        case LEX_GEQ:
            from_st(args, i);
            from_st(args, j);
            if (i == 's') {
                from_st(string_args, curr_str2);
                from_st(string_args, curr_str);
                args.push(curr_str >= curr_str2);
            }
            else args.push(j >= i);
            break;

        case LEX_NEQ:
            from_st(args, i);
            from_st(args, j);
            if (i == 's') {
                from_st(string_args, curr_str2);
                from_st(string_args, curr_str);
                args.push(curr_str != curr_str2);
            }
            args.push(j != i);
            break;

        case LEX_ASSIGN:
            from_st(args, i);
            from_st(args, j);
            if (TID[j].get_type() != LEX_STRING)
                TID[j].put_value(i);
            else {
                from_st(string_args, curr_str);
                TID[j].put_value(put_ttw(curr_str));
            }
            TID[j].put_assign();
            args.push(i);
            break;

        default:
            throw "POLIZ: unexpected elem";
        }//end of switch
        ++index;
    };//end of while
    cout << "Finish of executing!!!" << endl;
}

class Interpretator {
    Parser   pars;
    Executer E;
public:
    Interpretator(const char* program) : pars(program) {}
    void     interpretation();
};

void Interpretator::interpretation() {
    pars.analyze();
    E.execute(pars.poliz);
}

int main() {
    try {
        Interpretator I("string.txt");
        I.interpretation();
        return 0;
    }
    catch (char c) {
        cout << "unexpected symbol " << c << endl;
        return 1;
    }
    catch (Lex l) {
        cout << "unexpected lexeme" << l << endl;
        return 1;
    }
    catch (const char* source) {
        cout << source << endl;
        return 1;
    }
}