begin;

insert into version (version) values (9);


create table attendance.holidays (
    date date,
    description text not null
);


create table attendance.work_calendar (
    period date primary key,
    hours8 interval not null,
    check (extract(day from period) = 1)
);



commit;
