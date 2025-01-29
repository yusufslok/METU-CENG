import os
import secrets
from PIL import Image
from flask import url_for, current_app, jsonify
from flask_mail import Message
from flask_tutor import mail
import torch
from transformers import pipeline
from email_validator import validate_email, EmailNotValidError
from groq import Groq
import pickle
from sklearn.ensemble import RandomForestRegressor
import pandas as pd
import numpy as np


def save_picture(form_picture):
    random_hex = secrets.token_hex(8)
    _, f_ext = os.path.splitext(form_picture.filename)
    picture_fn = random_hex + f_ext
    picture_path = os.path.join(current_app.root_path, 'static/profile_pics', picture_fn)
    
    output_size = (125, 125)
    i = Image.open(form_picture)
    i.thumbnail(output_size)
    
    i.save(picture_path)

    return picture_fn

def send_reset_email(user):
    token = user.get_reset_token()
    msg = Message('Password Reset Request', sender=os.environ.get('EMAIL_USER'), 
                  recipients=[user.email])
    msg.body = f''' To reset your password, visit the following link:
{url_for('users.reset_token', token=token, _external=True)}

If you didn't make this request then simply ignore this email and no change has been made.
'''
    mail.send(msg)

def get_responded_text(prompt):

    try:
        response = current_app.qa_processor.ask_question(prompt)
        print(response)
        return response
    except Exception as e:
        return str(e)
    
def get_mental_health_analysis_response(history):
    api_key = "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"

    client = Groq(api_key=api_key)

    history.append({"role": "agent", "content": "Analyze the message history (messages prior to this one) to perform a mental health analysis of the person interacting with the chatbot."})
    
    try:
        response = client.chat.completions.create(
            model="llama-3.3-70b-versatile",
            messages=history,
            max_tokens=128
        )
        generated_text = response.choices[0].message.content
        return generated_text

    except Exception as e:
        return str(e)






    


