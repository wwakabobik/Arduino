from db.db import get_db

data_update_period = 5


def store_weather_data(data):
    connection = get_db()
    sql = f'''INSERT INTO weather_data(ts,meas_type,temperature,humidity,pressure,dew_point) VALUES({data});'''
    cur = connection.cursor()
    cur.execute(sql)
    connection.commit()
    return cur.lastrowid


def get_one_measurement(param, offset, meas_type=''):
    connection = get_db()
    if meas_type == 'temperature' or meas_type == 'humidity':
        criteria = f'WHERE meas_type = {meas_type}'
    else:
        criteria = ''
    sql = f''' SELECT {param} FROM weather_data ORDER BY RowId DESC {criteria} LIMIT 1 OFFSET {offset}; '''
    cur = connection.cursor()
    cur.execute(sql)
    row = cur.fetchone()
    if cur.rowcount > 0:
        retval = dict(zip(row.keys(), row))[param]
    else:
        retval = 0
    return retval


def get_last_series_average_measurement(param, period, meas_type=''):
    connection = get_db()
    if meas_type == 'temperature' or meas_type == 'humidity':
        criteria = f'WHERE meas_type = {meas_type}'
    else:
        criteria = ''
    number = str(period / data_update_period)
    sql = f''' avg({param}) from (SELECT param FROM weather_data {criteria} ORDER BY RowId DESC  LIMIT {number}); '''
    cur = connection.cursor()
    cur.execute(sql)
    row = cur.fetchone()
    if cur.rowcount > 0:
        retval = dict(zip(row.keys(), row))[param]
    else:
        retval = 0
    return retval


def get_last_series_measurement(param, period, meas_type=''):
    connection = get_db()
    if param == 'temperature' or param == 'humidity':
        criteria = f'WHERE meas_type = {meas_type}'
    else:
        criteria = ''
    number = int(period / data_update_period)
    sql = f''' SELECT ts, {param} FROM weather_data {criteria} ORDER BY RowId DESC  LIMIT {number}; '''
    cur = connection.cursor()
    cur.execute(sql)

    columns = [column[0] for column in cur.description]
    results = []
    for row in cur.fetchall():
        results.append(dict(zip(columns, row)))
    x = []
    y = []
    for result in results:
        x.append(result['ts'])
        y.append(result[param])
    return x, y
