begin;

insert into version (version) values (13);

create table attendance.summary (
    employee                integer not null references attendance.employees(employee) on update cascade on delete cascade,
    month                   date not null check (month = date_trunc('month', month)),
    days                    integer,
    work                    numeric,
    overtime                numeric,
    vacation                numeric,
    sick_leave              numeric,
    compensatory_leave      numeric,
    business_trip           numeric,
    break_time              numeric,
    unpaid_leave            numeric,
    sick_care               numeric,
    paid_obstacle           numeric,
    doctor                  numeric,
    afternoon               numeric,
    night                   numeric,
    sunday                  numeric,
    saturday                numeric,
    holiday                 numeric,
    locked                  boolean,
    locked_user             integer not null references users("user") on update cascade on delete set null,
    primary key (employee, month)
);

commit;
