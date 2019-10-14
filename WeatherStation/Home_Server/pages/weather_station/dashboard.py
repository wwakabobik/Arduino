from flask import render_template

from db.db import db_get_weather_data_one_measurement, data_update_period


def dashboard_page():

    # current data
    temperature_in = int(db_get_weather_data_one_measurement('temperature', '0', '0'))
    temperature_out = int(db_get_weather_data_one_measurement('temperature', '0', '1'))
    humidity_in = int(db_get_weather_data_one_measurement('humidity', '0', '0'))
    humidity_out = int(db_get_weather_data_one_measurement('humidity', '0', '1'))
    pressure = int(db_get_weather_data_one_measurement('pressure', '0'))
    dew_point = int(db_get_weather_data_one_measurement('dew_point', '0'))
    # previous data
    prev_temperature_in = int(db_get_weather_data_one_measurement('temperature', '1', '0'))
    prev_temperature_out = int(db_get_weather_data_one_measurement('temperature', '1', '1'))
    prev_humidity_in = int(db_get_weather_data_one_measurement('humidity', '1', '0'))
    prev_humidity_out = int(db_get_weather_data_one_measurement('humidity', '1', '1'))
    prev_pressure = int(db_get_weather_data_one_measurement('pressure', '1'))
    prev_dew_point = int(db_get_weather_data_one_measurement('dew_point', '1'))
    # last day data
    period = int(((60 * 24) / data_update_period) - 1)
    last_day_temperature_in = int(db_get_weather_data_one_measurement('temperature', f'{period}', '0'))
    last_day_temperature_out = int(db_get_weather_data_one_measurement('temperature', f'{period}', '1'))
    last_day_humidity_in = int(db_get_weather_data_one_measurement('humidity', f'{period}', '0'))
    last_day_humidity_out = int(db_get_weather_data_one_measurement('humidity', f'{period}', '1'))
    last_day_pressure = int(db_get_weather_data_one_measurement('pressure', f'{period}'))
    last_day_dew_point = int(db_get_weather_data_one_measurement('dew_point', f'{period}'))
    # last year data
    period = int(((60 * 24 * 365) / data_update_period) - 1)
    last_year_temperature_in = int(db_get_weather_data_one_measurement('temperature', f'{period}', '0'))
    last_year_temperature_out = int(db_get_weather_data_one_measurement('temperature', f'{period}', '1'))
    last_year_humidity_in = int(db_get_weather_data_one_measurement('humidity', f'{period}', '0'))
    last_year_humidity_out = int(db_get_weather_data_one_measurement('humidity', f'{period}', '1'))
    last_year_pressure = int(db_get_weather_data_one_measurement('pressure', f'{period}'))
    last_year_dew_point = int(db_get_weather_data_one_measurement('dew_point', f'{period}'))
    return render_template("weather_station/dashboard.html",
                           temperature_in=temperature_in,
                           temperature_out=temperature_out,
                           humidity_in=humidity_in,
                           humidity_out=humidity_out,
                           pressure=pressure,
                           dew_point=dew_point,
                           prev_temperature_in=prev_temperature_in,
                           prev_temperature_out=prev_temperature_out,
                           prev_humidity_in=prev_humidity_in,
                           prev_humidity_out=prev_humidity_out,
                           prev_pressure=prev_pressure,
                           prev_dew_point=prev_dew_point,
                           last_day_temperature_in=last_day_temperature_in,
                           last_day_temperature_out=last_day_temperature_out,
                           last_day_humidity_in=last_day_humidity_in,
                           last_day_humidity_out=last_day_humidity_out,
                           last_day_pressure=last_day_pressure,
                           last_day_dew_point=last_day_dew_point,
                           last_year_temperature_in=last_year_temperature_in,
                           last_year_temperature_out=last_year_temperature_out,
                           last_year_humidity_in=last_year_humidity_in,
                           last_year_humidity_out=last_year_humidity_out,
                           last_year_pressure=last_year_pressure,
                           last_year_dew_point=last_year_dew_point,
                           )
