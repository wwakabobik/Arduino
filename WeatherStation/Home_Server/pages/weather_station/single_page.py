from datetime import datetime

from flask import render_template
from plotly import graph_objs
import plotly

from db.db import db_get_weather_data_series_measurement, data_update_period


def single_page(param, period):
    # set proper type
    if "_in" in param:
        meas_type = 0
        fparam = param[:-3]
    elif "_out":
        meas_type = 1
        fparam = param[:-4]
    # set numeric period
    if period == "hour":
        num_period = int(60);
    elif period == "day":
        num_period = int(60 * 24)
    elif period == "month":
        num_period = int(60 * 24 * 30)
    elif period == "year":
        num_period = int(60 * 24 * 365)
    else:
        raise Exception('Wrong period', f'{period}')


    x, y = db_get_weather_data_series_measurement(period=num_period, param=fparam, meas_type=meas_type)

    x = [datetime(year=2013, month=10, day=4),
         datetime(year=2013, month=11, day=5),
         datetime(year=2013, month=12, day=6)]

    line_chart = graph_objs.Scatter(x=x, y=[1, 3, 6])])
    fig = graph_objs.Figure(data=line_chart)

    # Use datetime objects to set xaxis range
    fig.update_layout(xaxis_range=[datetime(2013, 10, 17),
                                   datetime(2013, 11, 20)])
    plotly
    return render_template("weather_station/single_page.html")

