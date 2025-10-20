/**
\page spg_web_program Web Programming 

\section wp_sec_overview Web-based UI Allocation View
\image html img/web_arch.png "Typical Web Application Architecture"

\section wp_sec_front Front-end Technologies
- Client-side
- Basically
    - HTML : contents
    - CSS : style
    - JavaScript : event & action + data
- Frameworks
    - jQuery : DOM object query
    - Twitter Bootstrap : style pattern
    - AngularJs 1, 2 :  single-page-application framework
    - ReactJs : View library
    - etc. (C3.js for chart)

\section wp_sec_back Back-end Technologies
- Server-side
- Database
    - SQL : mysql, oracle, postgre sql, ms sql, sqlite
    - NoSQL : mongodb
- Application server
    - python/Flask, Django :  python extensibility
    - node.js/Express : identical language
    - go/pat : fastest
- Web server for load balancing
    - not required yet

\section wp_sec_arch Proposed Architecture
- Front-end
    - HTML5/CSS3/ES6
    - AngularJs
    - Browser : CEF3 (or Chrome)
- Back-end
    - python/Flask
    - mongodb
    - boost.python binding

\section wp_sec_html HTML
- HTML is a declarative description of page
    -  https://www.w3schools.com/html/default.asp
- Imperative vs. Declarative Programming
    - imperative code describes a way to manipulate Sth
        - it does not guarantee result (you have to guarantee!)
        - typical language : c++, java
            - ex) drawimage(“dog.jpg”,  offsx, offsy,100, 100)
    - declarative code describes a state of Sth to be
        - it doesn’t care how to get the state (browser will do that somehow)
        - typical language : html, css            
    - javascript is a mix of imperative & declarative characteristics
        - selector and binding are declarative elements
        - function is an imperative element
- All html elements are treated as object and javascript can manipulate them
- To manipulate elements, understanding of DOM(Document Object Model) is required
    - https://www.w3schools.com/js/js_htmldom.asp
    - document,  elements, style, event, …
- Likewise DOM, understanding of BOM(browser object model) is required, to manipulate browser app.
    - https://www.w3schools.com/js/js_window.asp
    - windows, screen, url, history, navigator

\section wp_sec_css CSS
- CSS describes how HTML elements should be displayed
- Original CSS
    - https://www.w3schools.com/css/default.asp
- CSS3
    - the latest standard for CSS
    - backwards-compatible
    - https://www.w3schools.com/css/css3_intro.asp
- RDW
    - responsive web design makes a web page look good on all devices, using only HTML and CSS (no javascript)
    - https://www.w3schools.com/css/css_rwd_intro.asp

\section wp_sec_js JavaScript
- the programming language of HTML and the Web
    - and also used for server-side applications, s.t. node.js/Express
- tutorial sites
    - https://www.w3schools.com/js/default.asp
- ES6(ecmascript 2015)
    - significant update to earlier versions like C++11 to C++0X
    - most of modern language features adopted
        - https://babeljs.io/learn-es2015/
    - browser support check
        - http://kangax.github.io/compat-table/es6/

\section wp_sec_angular AngularJs
- an application framework for front-end application
- extends HTML with ng-directives
    - ng-app, ng-model, ng-bind, …
- features
    - templates
        - using directive, markup, filter, form controls
    - 2-way data binding
        - controller and template

- Study site
    - https://www.w3schools.com/angular/default.asp

\section wp_sec_flask Flask
- a microframework for python based on werkzeug, jinja2
    - Werkzeug(WSGI library) is like a communicator between python code and http nginx/apache server
- http://flask.pocoo.org/docs/0.12/
    - routing    
        \code{.py}
        @app.route('/')
        def index():
            return 'Index Page'

        @app.route('/hello')
        def hello():
            return 'Hello, World'
        \endcode
    - rendering templates    
        \code{.py}
        from flask import render_template

        @app.route('/hello/')
        @app.route('/hello/<name>')
        def hello(name=None):
            return render_template('hello.html', name=name)
        \endcode
    - access request data
        \code{.py}
        from flask import request

        with app.test_request_context('/hello', method='POST'):
            # now you can do something with the request until the
            # end of the with block, such as basic assertions:
            assert request.path == '/hello'
            assert request.method == 'POST'
        \endcode
    - sessions
        \code{.py}
        from flask import Flask, session, redirect, url_for, escape, request

        app = Flask(__name__)

        @app.route('/')
        def index():
            if 'username' in session:
                return 'Logged in as %s' % escape(session['username'])
            return 'You are not logged in'

        @app.route('/login', methods=['GET', 'POST'])
        def login():
            if request.method == 'POST':
                session['username'] = request.form['username']
                return redirect(url_for('index'))
            return '''
                <form method="post">
                    <p><input type=text name=username>
                    <p><input type=submit value=Login>
                </form>
            '''

        @app.route('/logout')
        def logout():
            # remove the username from the session if it's there
            session.pop('username', None)
            return redirect(url_for('index'))

        # set the secret key.  keep this really secret:
        app.secret_key = 'A0Zr98j/3yX R~XHH!jmN]LWX/,?RT'
        \endcode


\section wp_sec_jinja Jinja2
- a template language for python
    - example
        \code{.html}
        {% extends "layout.html" %}
        {% block title %}Scheduler Simulation Result List{% endblock %}
        {% block content %}
            <h2>List of Simulation Results</h2><br><br>
            <div class="col-md-12 table-responsive">
                <table border="1" class="table table-bordered ">
                    <tr><th>index</th>
                        <th width="200" align="left">Equipment</th>
                        <th width="300" align="left">Comment</th>
                        <th align="left">Build time</th>
                        <th>Run Result</th>
                        <th>Delete</th>
                    </tr>
                    {% for item in results %}
                        <tr>
                            <td align="center">{{loop.index}}</td>
                            <td>{{item['equipment']}}</td>
                            <td>{{item['comment']}}</td>
                            <td>{{item['time']}}</td>
                            <td><a href="/?uuid={{item['uuid']}}"> show </a> </td>
                            <th><a href="/delete?uuid={{item['uuid']}}">delete</a></th>
                        </tr>
                    {% endfor %}
                </table>
            </div>
        {% endblock %}
        \endcode
    - http://jinja.pocoo.org/docs/2.9/
- syntax
    - {% ... %} for Statements
    - {{ ... }} for Expressions to print to the template output
    - {# ... #} for Comments not included in the template output
    - \#  ... ## for Line Statements
- features
    - variables, context
    - filters, macro, context filter
    - template inheritance
    - python-like expressions : loop, condition, …
    - …

\section wp_sec_mongo PyMongo
- mongodb
    - nosql db : Not Only SQL
        - https://www.mongodb.com/
        - gui : https://robomongo.org/
    - document db :
        - document,  collection, database, mongodb server
        - document : key-value paired dictionary data
        - collection : group of documents, comparable to table in RDMS
        - database : physical container of collecitons
- pymongo
    - mongo db adapter for python
    - https://api.mongodb.com/python/current/
    - db access
        \code{.py}
        from pymongo import MongoClient
        client = MongoClient('localhost', 27017)
        db = client['test-database']
        collection = db['test-collection']
        \endcode
    - operations,
        - query, insertion, deletion, indexing, …

\section wp_sec_angular_env AngularJs 개발환경
- IDE
    - Jetbtrains WebStorm (requires license)
        - https://www.jetbrains.com/webstorm/
- Package manager
    - node.js
        - https://nodejs.org/ko/download/
        - ssl disable (사내 proxy때문)
            \code{.sh}
            npm config set strict-ssl false
            \endcode
        - install bower
            \code{.sh}
            npm install -g bower
            \endcode
- Automation (optional)
    - grunt
        \code{.sh}
        npm install -g grunt-cli
        \endcode

\section wp_sec_angular_sample AngularJs Sample
- Project 생성 (WebStorm)
    - __file__-->__new__-->__project__-->__AngularJs__
        - ex) create at ~/work/hello
- Ui Grid example
    - at ~/work/hello
    \code{.sh}
    bower install angular-ui-grid
    \endcode
        - if ssl problems,  add “"strict-ssl"  : false” into .bowerrc
    - ui grid samples
        - http://ui-grid.info/docs/#/tutorial
- More angular UI components
    - https://github.com/angular-ui/
        - ui-grid, ui-select,  bootstrap,  …

\section wp_sec_c3 Graphics Sample (C3.js)
- package install
    \code{.sh}
    bower install c3
    \endcode
- html
    \code{.html}
    <script src="bower_components/d3/d3.min.js"></script>
    <script src="bower_components/c3/c3.js"></script>
    <link rel="stylesheet" href="bower_components/c3/c3.css"/>
    <p> C3 Chart Example
        <button onclick="refresh_chart()">Refresh Chart</button>
    </p>
    <div id="linechart"></div>
    <script>
        var series = function (len) {
            let data = [];
            for(let i = 0; i < len; i++ ) {
                data.push(Math.random()*100);
            }
            return data;
        };

        var chart = c3.generate({
            bindto: '#linechart',
            size: {
                height: 240,
                width: 480
            },
            data: {
                columns: [
                    ['data1', ...series(10)],
                    ['data2', ...series(10)]
                ]
            }
        });
        function refresh_chart() {
            chart.load({
                columns: [
                    ['data1', ...series(10)],
                    ['data2', ...series(10)]
                ]
            });
        };
    </script>
    \endcode

\section wp_sec_ref References
- HTML5, CSS, JavaScript, Bootstrap, AngularJs
    - https://www.w3schools.com/
    - http://www.ecma-international.org/ecma-262/6.0/
    - https://babeljs.io/learn-es2015/
- Chromium Embedded Framework
    - https://bitbucket.org/chromiumembedded/cef
- Angular UI
    - https://github.com/angular-ui
- python/Flask
    - http://flask.pocoo.org
- mongodb
    - https://api.mongodb.com/python/current/ (api)
    - https://robomongo.org/ (gui)
- node.js, grunt, bower
    - https://nodejs.org/ko/
    - https://gruntjs.com/
    - https://bower.io/



*/