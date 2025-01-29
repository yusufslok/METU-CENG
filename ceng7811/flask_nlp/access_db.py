import sqlite3

conn = sqlite3.connect('instance/site.db')
cursor = conn.cursor()

cursor.execute("SELECT name FROM sqlite_master WHERE type='table';")
tables = cursor.fetchall()
print("Tables:", tables)

cursor.execute("PRAGMA table_info(User);")
columns = cursor.fetchall()
print("Columns in User table:", columns)

cursor.execute("SELECT * FROM User;")
users = cursor.fetchall()
print("User data:", users)

conn.close()