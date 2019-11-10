import PlayerStat_pb2
import socket
import threading
import time

sock = socket.socket()
sock.bind(("", 50000))
sock.listen(1000)

class KillStat:
    def __init__(self, kills, nickname):
        self.kills = kills
        self.nickname = nickname
        self.time = time.time()

class SurviveStat:
    def __init__(self, survive_time, nickname):
        self.survive_time = survive_time
        self.nickname = nickname
        self.time = time.time()

kill_rating = []
survive_rating = []
online_count = 0

def nickname_used(nickname):
    for elem in kill_rating:
        if elem.nickname == nickname:
            return True
    return False

def find_kill_place(nickname):
    for number in range(len(kill_rating)):
        if kill_rating[number].nickname == nickname:
            return number
    return -1

def find_survive_place(nickname):
    for number in range(len(survive_rating)):
        if survive_rating[number].nickname == nickname:
            return number
    return -1

def garbage_collector():
    while True:
        time.sleep(1)
        for elem in kill_rating:
            if (time.time() - elem.time > 10) & (elem.time != -1):
                kill_rating.remove(elem)
        for elem in survive_rating:
            if (time.time() - elem.time > 10) & (elem.time != -1):
                survive_rating.remove(elem)
    

def input_handler(conn, nickname):
    print("start input handler for " + nickname)
    try:
        while True:
            data = conn.recv(1024)
            info = PlayerStat_pb2.PlayerInfo()
            info.ParseFromString(data)
            print("New data from " + nickname)
            print(("  >> {} kills; {} time survived").format(info.kills, info.time_survived))
            print("")
            if info.live:
                if not nickname_used(nickname+"(live)"):
                    kill_rating.append(KillStat(info.kills, info.nickname + "(live)"))
                    survive_rating.append(SurviveStat(info.time_survived, info.nickname + "(live)"))
                else:
                    place = find_kill_place(nickname + "(live)")
                    kill_rating[place].kills = info.kills
                    kill_rating[place].time = time.time()
                    place = find_survive_place(nickname + "(live)")
                    survive_rating[place].survive_time = info.time_survived
                    survive_rating[place].time = time.time()
            else:
                if not nickname_used(nickname):
                    kill_rating.append(KillStat(info.kills, info.nickname))
                    kill_rating[len(kill_rating)-1].time = -1
                    survive_rating.append(SurviveStat(info.time_survived, info.nickname))
                    survive_rating[len(survive_rating)-1].time = -1
                else:
                    place = find_kill_place(nickname)
                    kill_rating[place].kills = max(kill_rating[place].kills, info.kills)
                    kill_rating[place].time = -1
                    place = find_survive_place(nickname)
                    survive_rating[place].survive_time = max(survive_rating[place].survive_time, info.time_survived)
                    survive_rating[place].time = -1

            kill_rating.sort(key = lambda KillStat: KillStat.kills, reverse=True)
            survive_rating.sort(key = lambda SurviveStat: SurviveStat.survive_time, reverse=True)
    except ConnectionResetError:
        print(nickname + " disconnected")


def rank_broadcast(conn, nickname):
    global online_count
    print("start rank broadcast for " + nickname)
    try:
        while True:
            rank_info = PlayerStat_pb2.RankInfo()
            for number in range(5):
                if number == len(kill_rating):
                    break
                stat = PlayerStat_pb2.PlayerKillStat()
                stat.kills = kill_rating[number].kills
                stat.nickname = kill_rating[number].nickname
                stat.kills_place = number+1
                rank_info.top5kill.append(stat)

            for number in range(5):
                if number == len(survive_rating):
                    break
                stat = PlayerStat_pb2.PlayerSurviveStat()
                stat.time_survived = survive_rating[number].survive_time
                stat.nickname = survive_rating[number].nickname
                stat.time_survived_place = number+1 
                rank_info.top5survive.append(stat)
        
            rank_info.online_count = online_count
            data = rank_info.SerializeToString()
            conn.send(data)
            time.sleep(1)
    except ConnectionResetError:
        online_count -= 1


print("start server")

thread = threading.Thread(target=garbage_collector, args=())
thread.start()

while True:
    conn, addr = sock.accept()
    data = conn.recv(1024)
    data = data.decode("utf-8")
    print("New player >> " + data)
    online_count += 1
    thread = threading.Thread(target=input_handler, args=(conn,data,))
    thread.start()
    thread2 = threading.Thread(target=rank_broadcast, args=(conn,data,))
    thread2.start()