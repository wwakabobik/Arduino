from flask import render_template


def index_page():
    return render_template("single_page.html")
