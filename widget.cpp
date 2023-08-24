#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_clicked()
{

    QTextStream cout(stdout, QIODevice::WriteOnly);
    QSet<QChar> all = {'+','-','*','/','=','%','^','&','|','<','>','!','#','?',':','~',';'};
    QSet<QChar> calc = {'+','-','*','/','=','%','^'};
    QSet<QChar> comp = {'&','|','<','>','!'};
    QSet<QChar> spec = {'#','?',',','.',':','~',';'};

    QChar indent = ' ';
    int count_ind = 0;
    bool need_space = false; //需要空格
    bool front_is_empty = true;

    int state = 0;   //0:普通状态     >0：圆括号内状态
    QVector<QString> prev;
    QVector<QString> cur;
    QString ans;




    QTextDocument* doc = ui->textEdit->document();
    int count = doc->blockCount();

    //按行读取到prev中
    for(int i = 0;i < count;i++){
        QTextBlock textline = doc->findBlockByNumber(i);
        QString str = textline.text();
        prev.push_back(str);
    }


    for(QString& str : prev){
        int n = str.size();
        cout << "first state " << state << " : ";
        QString cu = "";
        front_is_empty = true;

        for(int i = 0;i < n;i++){


            //控制开头缩进
            if(i == 0){
                while(str[i] == ' ')
                    i++;
                cu.fill(indent,count_ind * 4);
            }

            if(i > 0 && str[i-1] != ' ')
                front_is_empty = false;
            //进入for循环
            if(str.mid(i,3) == "for" && str[i+3] == ' '){
                cu += "for";
                i += 3;
                while(str[i] != '(' && i < n){
                    i++;
                }
            }
            //进入if语句
            if(str.mid(i,2) == "if" && str[i+2] == ' '){
                cu += "if";
                i += 2;
                while(str[i] != '(' && i < n){
                    i++;
                }
            }


            //检测圆括号状态
            if(str[i] == '(')
                state++;

            cout << state << " ";


            //进入圆括号状态
            if(state > 0){
                //检测到空
                if(str[i] == ' '){
                    //圆括号后第一个字符特判
                    if(str[i-1] != '('){
                        cu += ' ';
                     }
                     while(str[i] == ' ' && i < n){
                        i++;
                    }

                }
                //检测到符号
                if(all.find(str[i]) != all.end()){
                    if(str[i] == str[i+1]){
                        // == 等情况
                        if(str[i] == '='){
                            if(str[i-1] != ' ')
                                cu += ' ';
                            cu += str[i++];
                        }else{
                            // ++ -- 情况
                            cu += str[i++];
                        }
                    }else{
                        //当为 <= <> 等情况
                        if(str[i-1] != ' ')
                            cu += ' ';
                        if(all.find(str[i+1]) != all.end()){
                            cu += str[i++];

                        }
                        //当为 < > = 等情况 略过
                    }

                    if(str[i+1] != ' ' && str[i+1] != ')')
                        need_space = true;


                }

            }



            //普通状态
            if(state == 0){
                if(str[i] == ' '){
                     cu += ' ';
                     while(str[i] == ' ' && i < n){
                        i++;
                    }

                }

                //检测到符号
                if(all.find(str[i]) != all.end()){
                    if(str[i] == str[i+1]){
                        // == 等情况
                        if(str[i] == '='){
                            if(str[i-1] != ' ')
                                cu += ' ';
                            cu += str[i++];
                        }else{
                            // ++ -- 情况
                            cu += str[i++];
                        }
                    }else{
                        //当为 <= <> 等情况
                        if(str[i-1] != ' ')
                            cu += ' ';
                        if(all.find(str[i+1]) != all.end()){
                            cu += str[i++];

                        }
                        //当为 < > = 等情况 略过
                    }

                    if(str[i+1] != ' ' && str[i+1] != ')')
                        need_space = true;


                }
            }
            //检测到花括号
            if(str[i] == '{'){

                if(!cu.isEmpty() && !front_is_empty){
                    cur.push_back(cu);
                    cu = "";
                }
                cu.fill(indent,count_ind * 4);
                cu += '{';
                count_ind++;
                cur.push_back(cu);
                cu = "";
                i++;
                if(i == n)
                    continue;

            }

            //检测到字符和字符串表示
            if(str[i] == '\'' || str[i] == '"'){
                cu += str[i++];
                while(str[i] != '\'' && str[i] != '"'){
                    cu += str[i++];
                }
            }

            //检测到圆括回
            if(str[i] == ')' && state > 0){
                state--;
                //如果括回
                if(str[i-1] == ' ' && cu.at(cu.size()-1) != '(')
                    cu.chop(1);
            }

            //检测到方括回
            if(str[i] == ']'){
                if(str[i-1] == ' ' && cu.at(cu.size()-1) != '[')
                    cu.chop(1);
            }

            //检测到花括回
            if(str[i] == '}'){
                count_ind--;
                if(!cu.isEmpty() && !front_is_empty){
                    cur.push_back(cu);
                    cu = "";
                }
                cu.fill(indent,count_ind * 4);
                cu += '}';
                cur.push_back(cu);
                cu = "";
                i++;
                //已到最后一个字符
                if(i == n)
                    continue;

            }
//            if(str[i] == ';' && str[i-1] == ' '){
//                    cu.chop(1);
//            }



            //填补当前下标字符
            cu += str[i];

            //需要加空的情况
            if(need_space)
                cu += ' ';
            need_space = false;
        }

        cout << endl;
        cout.flush();
        //将修改后的当前行加入到vector中
        if(!cu.isEmpty())
           cur.push_back(cu);

    }


    for(QString str : cur){
        ans += str + '\n';

    }


    ui->textEdit->setText(ans);

    return;
}
