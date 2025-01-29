from flask import render_template, request, Blueprint, jsonify
from flask_login import current_user

main = Blueprint('main', __name__)

@main.route("/")
@main.route("/home")
def home():
    if not current_user.is_authenticated:
        if request.is_json:
            return jsonify({"message": "User not authenticated"}), 401
        return render_template('base_home.html')

    return render_template('chatbot.html')

@main.route("/about")
def about():
    return render_template('about.html', title='About')
