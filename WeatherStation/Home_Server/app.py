#!/usr/bin/env python3.7

from datetime import datetime

from flask import Flask, jsonify, request, abort

from db.db import db_store_weather_data, init_app
from pages.index import index_page
from pages.weather_station.dashboard import dashboard_page
from pages.weather_station.single_page import single_page

app = Flask(__name__)


@app.route('/api/v1/add_weather_data', methods=['POST'])
def store_in_db():
    if not request.json:
        abort(400)
    timestamp = str(datetime.now())
    data = request.json.get('data', "")
    db_data = f'"{timestamp}", {data}'
    ok=db_store_weather_data(db_data)
    print(ok)
    return jsonify({'data': db_data}), 201


@app.route('/')
@app.route('/index')
def index():
    return index_page()


@app.route('/dashboard')
def dashboard():
    return dashboard_page()


@app.route('/single_page')
def single():
    period = request.args.get('period')
    print(f"DEBUG: {period}")
    param = request.args.get('type')
    print(f"DEBUG: {param}")
    return single_page(param=param, period=period)


if __name__ == '__main__':
    init_app(app)
    app.run(debug=True)

