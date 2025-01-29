from flask import Flask
from flask_sqlalchemy import SQLAlchemy
from flask_bcrypt import Bcrypt
from flask_login import LoginManager
from flask_mail import Mail
from flask_tutor.config import Config
from qa_processor import QAProcessor  # Import QAProcessor

db = SQLAlchemy()
bcrypt = Bcrypt()
login_manager = LoginManager()
login_manager.login_view = 'users.login'
login_manager.login_message_category = 'info'
mail = Mail()

def create_app(config_class=Config):
    app = Flask(__name__)
    app.config.from_object(Config)

    db.init_app(app)
    bcrypt.init_app(app)
    login_manager.init_app(app)
    mail.init_app(app)

    # Initialize QAProcessor and attach it to the app instance
    raw_file_path = '/home/locki/Desktop/new_method/metu_data.txt'
    app.qa_processor = QAProcessor(raw_file_path)

    from flask_tutor.users.routes import users
    from flask_tutor.main.routes import main
    from flask_tutor.errors.handlers import errors
    app.register_blueprint(users)
    app.register_blueprint(main)
    app.register_blueprint(errors)

    return app
