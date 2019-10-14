#!/usr/bin/env python3.7

from datetime import datetime, date, time

from flask import Flask, jsonify, request, abort

from db.db import db_store_weather_data
from pages.index import index_page
from pages.weather_station.dashboard import dashboard_page

app = Flask(__name__)


@app.route('/api/v1/add_weather_data', methods=['POST'])
def store_in_db():
    if not request.json:
        abort(400)
    timestamp = str(datetime.now())
    data = request.json.get('data', "")
    db_data = f'{timestamp} {data}'
    db_store_weather_data(db_data)
    return jsonify({'data': db_data}), 201


if __name__ == '__main__':
    app.run(debug=True)


@app.route('/')
@app.route('/index')
def index():
    return index_page()


@app.route('/dashboard')
def dashboard():
    return dashboard_page()
