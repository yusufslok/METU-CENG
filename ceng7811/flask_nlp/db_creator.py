from run import app
from flask_tutor import db

with app.app_context():
    db.create_all()
    print("Database tables created successfully!")