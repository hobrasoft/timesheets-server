class Api {
    constructor() {
        this.onFinished = function(data) { }
        this.onError = function(errorText) { console.log(errorText); }
        this.apiPath = "/api/v1/";

        this.get = function(url, params) {
            var rq = new XMLHttpRequest();
            var todleto = this;
            rq.onerror = function() {
                todleto.onError(rq.responseText);
                };
            rq.onreadystatechange = function() {
                if (rq.readyState === XMLHttpRequest.DONE && rq.status == 200) {
                    todleto.onFinished(JSON.parse(rq.responseText));
                    }
                };
            params = typeof params === 'undefined' ? '' : '?'+params;
            var completeUrl = this.apiPath + url + params;
            rq.open("GET", completeUrl, true);
            rq.send();
            }

        this.put = function(url, data) {
            var rq = new XMLHttpRequest();
            var todleto = this;
            rq.onerror = function() {
                console.log(rq.responseText);
                todleto.onError(rq.responseText);
                };
            rq.onreadystatechange = function() {
                if (rq.readyState === XMLHttpRequest.DONE && (rq.status == 200 || rq.status == 204)) {
                    todleto.onFinished(JSON.parse(rq.responseText));
                    }
                };
            // var completeUrl = initpage.serverUrl + initpage.apiPath + url + "?user=" + initpage.username + "&password=" + initpage.password;
            var completeUrl = this.apiPath + url;
            rq.open("PUT", completeUrl, true);
            rq.setRequestHeader("Content-Type", "application/json; charset=UTF-8");
            rq.send(data);
            }

        this.delete = function(url) {
            var rq = new XMLHttpRequest();
            var todleto = this;
            rq.onerror = function() {
                console.log(rq.responseText);
                todleto.onError(rq.responseText);
                };
            rq.onreadystatechange = function() {
                if (rq.readyState === XMLHttpRequest.DONE && (rq.status == 200 || rq.status == 201)) {
                    todleto.onFinished(JSON.parse(rq.responseText));
                    }
                };
            var completeUrl = this.apiPath + url;
            rq.open("DELETE", completeUrl, true);
            rq.send();
            }

        this.category = function (c) { this.get("categories/"+c); }
        this.categories = function () { this.get("categories"); }
        this.categoriesToRoot = function (c) { this.get("categoriestoroot/"+c); }
        this.categoriestree = function (category) { this.get("categoriestree/"+category,"maxdepth=0"); }
        this.ticketsvw = function (category) { this.get("ticketsvw", "category=" + category); }
        this.ticketsvwall = function () { this.get("ticketsvw", ""); }
        this.ticketvw = function (ticket) { this.get("ticketsvw/"+ticket+"?all=true"); }
        this.saveCategory = function (c) { this.put("categories/", JSON.stringify(c)); }
        this.statuses = function (category, prevstatus) { this.get("statuses", "category="+category+"&previousStatus="+prevstatus); }
        this.user = function (user) { this.get("users/" + user); }
        this.users = function (user) { this.get("users"); }
        this.saveTicket = function(t) { this.put("ticketsvw/", JSON.stringify(t)); }
        this.startTimesheet = function(t) { this.get("timesheet/start/" + t); }
        this.stopTimesheet = function(t) { this.get("timesheet/stop/" + t); }
        this.removeTicket = function(t) { this.delete("tickets/" + t); }
        this.removeCategory = function(c) { this.delete("categories/" + c); }
        this.authenticate = function(user, password) { this.get("authenticate"); }
        this.statusesAll = function () { this.get("statuses"); }
        this.overview = function (category, statuses) { this.get("overview/" + category,  "statuses=" + statuses.join(",")); }
        }
}


