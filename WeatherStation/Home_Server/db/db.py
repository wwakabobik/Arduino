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

    with current_app.open_resource('db/schema.sql') as f:
        db.executescript(f.read().decode('utf8'))


@click.command('init-db')
@with_appcontext
def init_db_command():
    """Clear the existing data and create new tables."""
    init_db()
    click.echo('Initialized the database.')


def init_app(app):
    app.teardown_appcontext(close_db)
    app.cli.add_command(init_db_command)


def db_store_weather_data(data):
    connection = get_db()
    sql = f'''INSERT INTO weather_data(ts,meas_type,temperature,humidity,pressure,dew_point) VALUES({data});'''
    cur = connection.cursor()
    cur.execute(sql)
    connection.commit()
    return cur.lastrowid


def db_get_weather_data_one_measurement(param, offset, meas_type=''):
    connection = get_db()
    if meas_type == 'temperature' or meas_type == 'humidity':
        criteria = f'WHERE meas_type = {meas_type}'
    else:
        criteria = ''
    sql = f''' SELECT {param} FROM weather_data ORDER BY RowId DESC {criteria} LIMIT 1 OFFSET {offset}; '''
    cur = connection.cursor()
    cur.execute(sql)
    row = cur.fetchone()
    print(f'DEBUG: {cur.rowcount}')
    if cur.rowcount > 0:
        retval = dict(zip(row.keys(), row))[param]
    else:
        retval = 0
    return retval


def db_get_weather_data_average_measurement(param, period, meas_type=''):
    connection = get_db()
    if meas_type == 'temperature' or meas_type == 'humidity':
        criteria = f'WHERE meas_type = {meas_type}'
    else:
        criteria = ''
    number = str(period / data_update_period)
    sql = f''' avg({param}) from (SELECT param FROM weather_data ORDER BY RowId DESC {criteria} LIMIT {number}); '''
    cur = connection.cursor()
    cur.execute(sql)
    row = cur.fetchone()
    if cur.rowcount > 0:
        retval = dict(zip(row.keys(), row))[param]
    else:
        retval = 0
    return retval


def db_get_weather_data_series_measurement(param, meas_type=''):
    connection = get_db()
    if param == 'temperature' or param == 'humidity':
        criteria = f'WHERE meas_type = {meas_type}'
    number = str(60 / data_update_period)
    sql = f''' SELECT ts, {param} FROM weather_data ORDER BY RowId DESC {criteria} LIMIT {number}; '''
    cur = connection.cursor()
    cur.execute(sql)
    rows = cur.fetchall()
    return dict(zip(rows.keys(), rows))
