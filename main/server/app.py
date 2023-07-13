from flask import Flask,request,render_template
from pymongo import MongoClient

app = Flask(__name__)

app.config['MONGO_URI'] = 'mongodb+srv://<username>:<password>@<cluster-url>/<database>?retryWrites=true&w=majority'

mongo = MongoClient(app.config['MONGO_URI'])

app.id=-1
app.text=''
app.data=''
app.name=''

@app.route('/register')
def func():
    return render_template('index.html')

@app.route('/id_data')
def home():
    app.name = request.args.get('name','') 
    app.id=request.args.get('id','')
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

if __name__ == '__main__':
    app.run(debug=True,host='192.168.123.96',port=8000)
