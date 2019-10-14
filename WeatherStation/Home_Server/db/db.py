import sqlite3

import click
from flask import current_app, g
from flask.cli import with_appcontext


data_update_period = 5


def get_db():
    if 'db' not in g:
        g.db = sqlite3.connect(
            "flask_db",
            detect_types=sqlite3.PARSE_DECLTYPES
        )
        g.db.row_factory = sqlite3.Row

    return g.db


def close_db(e=None):
    db = g.pop('db', None)

    if db is not None:
        db.close()


def init_db():
    db = get_db()

    with current_app.open_resource('schema.sql') as f:
        db.executescript(f.read().decode('utf8'))


@click.command('init-db')
@with_appcontext
def init_db_command():
    """Clear the existing data and create new tables."""
    init_db()
    click.echo('Initialized the database.')


def db_store_weather_data(data):
    connection = get_db()
    sql = f''' INSERT INTO weather_data(ts,type,temperature,humidity,pressure,dew_point) VALUES(?,?,?,?,?,?) '''
    cur = connection.cursor()
    cur.execute(sql, data)
    cur.commit()


def db_get_weather_data_one_measurement(param, number, meas_type=''):
    connection = get_db()
    if meas_type == 'temperature' or meas_type == 'humidity':
        criteria = f'WHERE meas_type = {meas_type}'
    sql = f''' SELECT NTH_VALUE({param},{number}) FROM weather_data ORDER BY RowId DESC {criteria} LIMIT {number} '''
    cur = connection.cursor()
    cur.execute(sql)
    return cur.fetchone()


def db_get_weather_data_average_measurement(param, period, meas_type=''):
    connection = get_db()
    if meas_type == 'temperature' or meas_type == 'humidity':
        criteria = f'WHERE meas_type = {meas_type}'
    number = str(period / data_update_period)
    sql = f''' avg({param}) from (SELECT param FROM weather_data ORDER BY RowId DESC {criteria} LIMIT {number}) '''
    cur = connection.cursor()
    cur.execute(sql)
    return cur.fetchone()


def db_get_weather_data_series_measurement(param, meas_type=''):
    connection = get_db()
    if param == 'temperature' or param == 'humidity':
        criteria = f'WHERE meas_type = {meas_type}'
    number = str(60 / data_update_period)
    sql = f''' SELECT ts, {param} FROM weather_data ORDER BY RowId DESC {criteria} LIMIT {number} '''
    cur = connection.cursor()
    cur.execute(sql)
    return cur.fetchall()
