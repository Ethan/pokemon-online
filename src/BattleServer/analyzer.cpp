#include "../Shared/networkcommands.h"
#include "../Utilities/network.h"
#include "../PokemonInfo/battlestructs.h"

#include "analyzer.h"

Analyzer::Analyzer(GenericSocket sock, int id) : socket(new Network<GenericSocket>(sock, id)), m_id(id)
{
    socket->setParent(this);

    connect(socket, SIGNAL(disconnected()), SIGNAL(disconnected()));
    connect(socket, SIGNAL(isFull(QByteArray)), this, SLOT(commandReceived(QByteArray)));
    connect(socket, SIGNAL(_error()), this, SLOT(error()));

    connect(this, SIGNAL(sendCommand(QByteArray)), socket, SLOT(send(QByteArray)));
}

void Analyzer::error()
{
    emit connectionError(socket->error(), socket->errorString());
}

void Analyzer::commandReceived(const QByteArray &command)
{
    dealWithCommand(command);
}

void Analyzer::dealWithCommand(const QByteArray &commandline)
{
    DataStream in (commandline);
    uchar command;

    in >> command;

    qDebug() << "Received command " << int(command);

    switch (command) {
    case EngageBattle: {
        qDebug() << "Engaging battle";
        qint32 battleid;
        BattlePlayer p1, p2;
        ChallengeInfo c;
        TeamBattle t1, t2;

        in >> battleid >> p1 >> p2 >> c >> t1 >> t2;

        emit newBattle(battleid, p1, p2, c, t1, t2);
        break;
    }
    case KeepAlive: {
        notify(KeepAlive);
        break;
    }
    default:
        //emit protocolError(UnknownCommand, tr("Protocol error: unknown command received"));
        break;
    }
}