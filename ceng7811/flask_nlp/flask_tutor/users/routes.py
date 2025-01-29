from flask import render_template, url_for, flash, redirect, request, Blueprint, jsonify, session
from flask_login import login_user, current_user, logout_user, login_required
from flask_tutor import db, bcrypt
from flask_tutor.models import User
from flask_tutor.users.forms import (RegistrationForm, LoginForm, UpdateAccountForm,
                                   RequestResetForm, ResetPasswordForm, PromptForm)
from flask_tutor.users.utils import save_picture, send_reset_email, get_responded_text, get_mental_health_analysis_response

users = Blueprint('users', __name__)

@users.route("/register", methods=['GET', 'POST'])
def register():
    if request.method == 'POST' and request.is_json:
        data = request.get_json()

        user_name = data['user_name']
        email = data['email']
        password = data['password']
        password_verification = data['password_verification']

        if password != password_verification:
            return jsonify({"success": False, "message": "Passwords do not match", "result_code": 400})

        if User.query.filter_by(email=email).first():
            return jsonify({"success": False, "message": "Email already registered", "result_code": 400})
        if User.query.filter_by(username=user_name).first():
            return jsonify({"success": False, "message": "Username already taken", "result_code": 400})

        hashed_pw = bcrypt.generate_password_hash(password).decode('utf-8')
        user = User(username=user_name, email=email, password=hashed_pw)
        db.session.add(user)
        db.session.commit()

        return jsonify({"success": True, "message": "Registration successful", "result_code": 201})

    if request.method == 'POST':
        if current_user.is_authenticated:
            return redirect(url_for('main.home'))
        form = RegistrationForm()
        if form.validate_on_submit():
            hashed_pw = bcrypt.generate_password_hash(form.password.data).decode('utf-8')
            user = User(username=form.username.data, email=form.email.data, password=hashed_pw)
            db.session.add(user)
            db.session.commit()
            flash(f'Your account has been created! You are now able to log in.', 'success')
            return redirect(url_for('users.login'))
        return render_template('register.html', title="Register", form=form)

    form = RegistrationForm()
    return render_template('register.html', title="Register", form=form)

@users.route("/login", methods=['GET', 'POST'])
def login():
    if request.method == 'POST' and request.is_json:
        data = request.get_json()
        email = data.get('email')
        password = data.get('password')

        if not email or not password:
            return jsonify({"success": False, "message": "Missing required fields", "result_code": 400})

        user = User.query.filter_by(email=email).first()
        if user and bcrypt.check_password_hash(user.password, password):
            login_user(user)
            return jsonify({"success": True, "message": "Login successful", "result_code": 200})
        else:
            return jsonify({"success": False, "message": "Invalid email or password", "result_code": 401})

    if request.method == 'POST':
        if current_user.is_authenticated:
            return redirect(url_for('main.home'))
        form = LoginForm()
        if form.validate_on_submit():
            user = User.query.filter_by(email=form.email.data).first()
            if user and bcrypt.check_password_hash(user.password, form.password.data):
                login_user(user, remember=form.remember.data)
                next_page = request.args.get('next')
                return redirect(next_page) if next_page else redirect(url_for('main.home'))
            else:
                flash('Login Unsuccessful. Please check email and password', 'danger')
        return render_template('login.html', title="Login", form=form)

    form = LoginForm()
    return render_template('login.html', title="Login", form=form)


@users.route("/logout", methods=['POST', 'GET'])
def logout():
    logout_user()

    if request.is_json:
        return jsonify({"message": "Logout successful"}), 200

    return redirect(url_for('main.home'))


@users.route("/account", methods=['GET', 'POST'])
@login_required
def account(): # it doesnt check whether the updated email, username are already in use
    form = UpdateAccountForm()
    if form.validate_on_submit():
        if form.picture.data:
            picture_file = save_picture(form.picture.data)
            current_user.image_file = picture_file
        current_user.username = form.username.data
        current_user.email = form.email.data
        db.session.commit()
        flash('Your account has been updated!', 'success')
        return redirect(url_for('users.account'))
    elif request.method == 'GET':
        form.username.data = current_user.username
        form.email.data = current_user.email
    image_file = url_for('static', filename='profile_pics/' + current_user.image_file)
    return render_template('account.html', title="Account", 
                            image_file=image_file, form=form)

@users.route("/user/<string:username>")
def user_inputs(username):
    page = request.args.get('page', 1, type=int)
    user = User.query.filter_by(username=username).first_or_404()
    return render_template('user_inputs.html', user=user)

@users.route("/reset_password", methods=['GET', 'POST'])
def reset_request():
    if current_user.is_authenticated:
        return redirect(url_for('main.home'))
    form = RequestResetForm()
    if form.validate_on_submit():
        user = User.query.filter_by(email=form.email.data).first()
        send_reset_email(user)
        flash('An email has been sent with instructions to reset your password!', 'info')
        return redirect(url_for('users.login'))
    return render_template('reset_request.html', title='Reset Password', form=form)

@users.route("/reset_token/<token>", methods=['GET', 'POST'])
def reset_token(token):
    if current_user.is_authenticated:
        return redirect(url_for('main.home'))
    user = User.verify_reset_token(token)
    if not user:
        flash('That is an invalid or expired token!', 'warning')
        return redirect(url_for('users.reset_request'))
    form = ResetPasswordForm()
    if form.validate_on_submit():
        hashed_pw = bcrypt.generate_password_hash(form.password.data).decode('utf-8')
        user.password = hashed_pw
        db.session.commit()
        flash(f'Your password has been updated! You are now able to log in.', 'success')
        return redirect(url_for('users.login'))
    return render_template('reset_token.html', title='Reset Password', form=form)

@users.route("/chatbot", methods=['GET', 'POST'])
def chatbot():
    return render_template('chatbot.html', title='Chatbot')

@users.route("/chatbot/get_respond", methods=['POST'])
@login_required
def get_respond():
    data = request.get_json()
    if not data or 'prompt' not in data:
        return jsonify({'error': 'Invalid request data'})

    prompt = data['prompt']

    if 'history' not in session:
        session['history'] = [
            {"role": "system", "content": "Hi, I'm your personal METU tutor. How can I help you?"}
        ]

    print(session['history'])   

    session['history'].append({"role": "user", "content": prompt})

    bot_response = get_responded_text(prompt)

    session['history'].append({"role": "assistant", "content": bot_response})

    session.modified = True

    return jsonify({'content': bot_response, 'role': 'assistant', 'success': True})


@users.route("/chatbot/get_history", methods=['GET'])
def get_history():
    history = session.get('history', [])
    first_message = {'content': "Hi, I'm your personal METU tutor. How can I help you?", 'role': 'system'}
    modified_history = [first_message] + history[1:]
    return jsonify(modified_history)

@users.route("/chatbot/clear_history", methods=['POST'])
def clear_history():
    session.pop('history', None)  
    session.modified = True
    return jsonify({"message": "History cleared successfully"})

@users.route("/chatbot/get_mental_health_analysis", methods=["POST"])
def get_mental_health_analysis():
    data = request.get_json()
    history = data.get("history", [])
    
    # Call your existing function to get the analysis
    analysis = get_mental_health_analysis_response(history)  # Assuming this returns the AI-generated response
    return jsonify({"mental_health_analysis": analysis})

