from flask import Flask, render_template, request, jsonify

app = Flask(__name__)

@app.route('/')
def index():
	name = request.args.get("name")
	return jsonify( "name" : 'name' )


@app.route("/process", methods=["GET"])
def process():
	name = request.args.get("name")
	return jsonify( "name" : 'name' )

process()


# if __name__ == '__main__'
# 	app.run(debug=True)