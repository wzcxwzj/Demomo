//#pragma execution_character_set("utf-8")
#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QString>
#include <QDebug>
#include <stdio.h>
#include <stdlib.h>
#include <QDomDocument>
#include <QtXml>
#include <QCryptographicHash>
#include <QFile>
#include <QDir>


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    connect(ui->textEdit,SIGNAL(textChanged()),this,SLOT(getText()));
    QDateTime curDateTime=QDateTime::currentDateTime();
    currentTime=curDateTime.toString("yyyy-MM-dd");
}

Widget::~Widget()
{
    delete ui;
}

void Widget::getText()
{

    QString str=ui->textEdit->toPlainText();
    QStringList strList=  str.split("file:///");

    QStringList pathList;
    for(int i=0;i<strList.size();i++)
    {
        //先干掉"\n"
       QStringList pp=  strList[i].split("\n");
       pathList<<pp[0];
    }
    for(int i=0;i<pathList.size();i++)
    {
        QFileInfo fileInfo(pathList[i]);
        if(fileInfo.isDir())                            //目录
        {
            FindFile(pathList[i]);                      //获得目录下全部的文件  并保存到dirPathParse.
        }
        if(fileInfo.isFile())                           //文件
        {

            fileList<<pathList[i];

        }
    }

    //文件名解析  因为要对文件名进行md5加密

    for(int i=0;i<dirPathParse.size();i++)              //目录下的MD5
    {
        QByteArray strp= getFileMd5(dirPathParse[i]);
         md5List<<QString(strp.toHex().constData());
    }

    for(int i=0;i<fileList.size();i++)                  //文件的MD5
    {
        QByteArray strp= getFileMd5(fileList[i]);
        md5FileList<<QString(strp.toHex().constData());

    }

    for(int i=0;i<dirPathParse.size();i++)
    {
        QString AA=dirPathParse.at(i);

        QStringList BB=AA.split(/*"C:/Users/Administrator/Desktop/"*/relativePath);  //存放相对路径的地方
        pDir<<BB[1];
    }

    //xml文件名

    for(int i=0;i<fileList.size();i++)          //***取最后一个文件名
    {
        QString  title=fileList.at(i);

        QStringList pFile=title.split(/*"C:/Users/Administrator/Desktop/"*/relativePath);;      //存放相对路径的地方
        titleList<<pFile[1];
    }
}
void Widget::writeXml()
{
    QByteArray array;
    QDomDocument doc;
    QDomProcessingInstruction instruction;
    QList<QDomText> elementText;

    instruction = doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");

    doc.appendChild(instruction);                       //doc开头加入instruction

    QDomElement root = doc.createElement("root");       //创建root节点
    QDomElement version = doc.createElement("VERSION");   //创建节点common
    QDomElement updatetime = doc.createElement("UPDATETIME");   //创建节点common
    QDomElement describe = doc.createElement("DESCRIBE");   //创建节点common

    elementText<<doc.createTextNode(ui->lineEdit->text());
    version.appendChild(elementText[0]);
    elementText<<doc.createTextNode(currentTime);
    updatetime.appendChild(elementText[1]);
    QString descript=ui->lineEdit_2->text();
   // QString str=QString::fromLocal8Bit(descript);
    elementText<<doc.createTextNode(descript);
    describe.appendChild(elementText[2]);
    root.appendChild(version);
    root.appendChild(updatetime);
    root.appendChild(describe);

    QList<QDomElement>md5;
    QList<QDomElement>name;
    QList<QDomText>elementTextName;
    int i;
    for(i=0;i<md5List.size();i++)                           //目录下xml
    {
        md5<<doc.createElement("MD5");   //创建节点common
        md5[i].setAttribute("id", md5List[i]);              //MD5值
        name<<doc.createElement("name");                    //MD5的子节点

        elementTextName<<doc.createTextNode(pDir[i]);       //name节点的值
        name[i].appendChild(elementTextName[i]);

        md5[i].appendChild(name[i]);                        //name加入MD5
        root.appendChild(md5[i]);
    }

    for(int j=0;j<md5FileList.size();j++)                   //文件xml
    {
        md5<<doc.createElement("MD5");                      //创建节点common
        md5[i+j].setAttribute("id", md5FileList[j]);        //MD5值
        name<<doc.createElement("name");                    //MD5的子节点

        elementTextName<<doc.createTextNode(titleList[j]);  //name节点的值
        name[i+j].appendChild(elementTextName[i+j]);

        md5[i+j].appendChild(name[i+j]);                    //name加入MD5
        root.appendChild(md5[i+j]);
    }

    doc.appendChild(root);
    array = doc.toByteArray();
    printf("%s", array.data());

    //将xml保存到文件中
    QFile file("./Description.xml");

    file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
    QTextStream out(&file);

    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec *codec = QTextCodec::codecForName("GBK");
    out.setCodec(codec);
    doc.save(out, 4, QDomNode::EncodingFromTextStream);

    file.close();
}


QByteArray Widget::getFileMd5(QString filePath)
{
    QFile localFile(filePath);

    if (!localFile.open(QFile::ReadOnly))
    {
        qDebug() << "file open error.";
        return 0;
    }

    QCryptographicHash ch(QCryptographicHash::Md5);

    quint64 totalBytes = 0;
    quint64 bytesWritten = 0;
    quint64 bytesToWrite = 0;
    quint64 loadSize = 1024 * 4;
    QByteArray buf;

    totalBytes = localFile.size();
    bytesToWrite = totalBytes;

    while (1)
    {
        if(bytesToWrite > 0)
        {
            buf = localFile.read(qMin(bytesToWrite, loadSize));
            ch.addData(buf);
            bytesWritten += buf.length();
            bytesToWrite -= buf.length();
            buf.resize(0);
        }
        else
        {
            break;
        }

        if(bytesWritten == totalBytes)
        {
            break;
        }
    }

    localFile.close();
    QByteArray md5 = ch.result();

    return md5;
}




bool Widget::FindFile(const QString &path)
{
    QDir dir(path);
    if (!dir.exists())
    {
        return false;
    }
    dir.setFilter(QDir::Dirs | QDir::Files);
    dir.setSorting(QDir::DirsFirst);//文件夹排在前面
    QFileInfoList list = dir.entryInfoList();
    int i = 0;

    bool bIsDir;
    do
    {
        QFileInfo fileInfo = list.at(i);
        QString strFilename = fileInfo.fileName();
        uint gid = fileInfo.groupId();
        QString strDateTime = fileInfo.lastModified().toString("yyyy-MM-dd hh:mm:ss");
        QString strPath = fileInfo.absoluteFilePath();
        if (fileInfo.fileName() == "." | fileInfo.fileName() == "..")
        {
            ++i;
            continue;
        }
        bIsDir = fileInfo.isDir();
        if (bIsDir)//如果是文件夹
        {
            ++m_nDirCount;//目录个数
            FindFile(fileInfo.filePath());
        }
        else
        {
            ++m_nFilesCount;//文件个数
            dirPathParse<<fileInfo.absoluteFilePath();

            int first=fileInfo.absolutePath().lastIndexOf("/");

            QString  title=fileInfo.absolutePath().\
                    right(fileInfo.absolutePath().length()-first-1);

            QStringList pFile=fileInfo.absolutePath().split(title);

            relativePath= pFile[0];
            qDebug()<<relativePath;
        }

        ++i;
    } while (i < list.size());


    return true;

}







void Widget::on_pushButton_clicked()
{
     writeXml();
}
