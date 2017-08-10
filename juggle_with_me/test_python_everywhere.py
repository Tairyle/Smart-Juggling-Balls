from flask import Flask, render_template, request, jsonify, make_response, request, current_app
from flask_sqlalchemy import SQLAlchemy
from sqlalchemy import ForeignKey
from flask_cors import CORS, cross_origin

app = Flask(__name__)
app.config["DEBUG"] = True
CORS(app)


# sqlalchemy part.
SQLALCHEMY_DATABASE_URI = "mysql+mysqlconnector://{username}:{password}@{hostname}/{databasename}".format(
    username="ceilingphan",
    password="1qaz!QAZ",
    hostname="ceilingphan.mysql.pythonanywhere-services.com",
    databasename="ceilingphan$databasenamesmartjuggling",
)
app.config["SQLALCHEMY_DATABASE_URI"] = SQLALCHEMY_DATABASE_URI
app.config["SQLALCHEMY_POOL_RECYCLE"] = 299
app.config["SQLALCHEMY_TRACK_MODIFICATIONS"] = False

db = SQLAlchemy(app)

class trick(db.Model):

    __tablename__ = "trick"

    trick_id = db.Column(db.Integer, primary_key=True, autoincrement=True)
    name = db.Column(db.String(4096))

class siteswap(db.Model):

    __tablename__ = "siteswap"

    siteswap_id = db.Column(db.Integer, primary_key=True, autoincrement=True)
    notation = db.Column(db.String(4096))
    trick_id = db.Column(db.Integer, ForeignKey(trick.trick_id) )

@app.route('/')
def index():
    # cur = mysql.connection.cursor()
    return render_template('form.html' )

@app.route('/dragon', methods=['GET', 'POST'])
def dragon():   
    sw = request.form['siteswap']
    thing = str(sw)

    filled_dict = {
        "333": "Cascade",
        "51" : "Shower"
    }
    
    if thing in filled_dict:
        return jsonify({'Sunday' : filled_dict[thing]})
    else:
        return jsonify({'Sunday' : "drop"})


if __name__== "__main__":
    app.run(debug=True)