from flask import Flask,request,render_template
from pymongo import MongoClient
import socket
from datetime import datetime

hostname=socket.gethostname()

app = Flask(__name__)

client = MongoClient('mongodb+srv://shariharan182003:asdf123%40ASDF@cluster1.h4nhum8.mongodb.net/')
db = client['fingerprint']
users = db['users']
entry = db['entries']

app.id=-1
app.text=''
app.data=''
app.recdata=''
app.name=''
app.decide=-1
app.file='index.html'

@app.route('/')
def begin():
    return render_template('begin.html')

@app.route('/decide')
def decide():
    app.decide=request.args.get('text','')
    app.file='index.html'
    if(app.decide!='0'):
        app.file='recognize.html'
    print(app.file)
    return app.file

@app.route('/deciderender')
def render():
    print(app.decide)
    return render_template(app.file)

@app.route('/decidedata')
def decidedata():
    app.text = request.args.get('text','') 
    if(app.text=='decide'):
        if(app.decide!=-1):
            print('decide')
            temp=app.decide
            app.decide=-1
            return str(temp)
    return str(app.decide)

@app.route('/id_data')
def home():
    app.name = request.args.get('name','') 
    app.id=request.args.get('id','')
    count=0
    data=users.find({"id":app.id})
    for i in data:
        count=count+1
        users.update_one({"id":app.id},{"$set":{"name":app.name}})
    if(count==0):
        document = {"name": app.name, "id": app.id}
        users.insert_one(document)
    return render_template('index1.html')

@app.route('/req',methods=['GET'])
def hello_world():
    app.text = request.args.get('text','') 
    if(app.text=='id'):
        if(app.id!=-1):
            print('id')
            temp=app.id
            app.id=-1
            return str(temp)
    return str(app.id)

@app.route('/msg',methods=['GET'])
def welcome():
    app.data = request.args.get('text','') 
    print(app.data)
    return 'none'

@app.route('/fetchdata',methods=['GET'])
def fetch():
    return app.data

@app.route('/rec',methods=['GET'])
def recg():
    app.recdata = request.args.get('text','') 
    temp = app.recdata.split(" ")
    # print(temp)
    # id=0
    if(temp[0]=="id"):
        id=int(temp[1])
        print(id)
        current_datetime = datetime.now()
        timestamp = current_datetime.timestamp()
        document = {"time": timestamp, "id": id}
    #     entry.insert_one(document)
    # print(temp)
    client.close()
    return 'none'

@app.route('/recgfetch',methods=['GET'])
def recgfetch():
    return app.recdata


if __name__ == '__main__':
    app.run(debug=True,host=hostname,port=8000)
