begin;

insert into version (version) values (10);


drop table if exists attendance.work_calendar;
create table attendance.work_calendar (
    period date not null primary key,
    working_days integer not null,
    holidays integer not null,
    hours8  interval not null,
    hours85 interval not null
);

create or replace function attendance.generate_work_calendar(year integer)
returns void as $$
begin
    insert into attendance.work_calendar(period, working_days, holidays, hours8, hours85)
    with days as (
        select date_trunc('month', d::date)::date as period, d::date as day
        from generate_series(
            make_date(year, 1, 1), make_date(year, 12, 31),
            interval '1 day'
        ) as d
    ),
    workdays as (
        select period,
               count(*) as working_days
        from days
        left join attendance.holidays h on h.date = days.day
        where extract(dow from days.day) between 1 and 5
          and h.date is null
        group by period
    ),
    holidays as (
        select date_trunc('month', h.date)::date as period,
               count(*) as holidays
        from attendance.holidays h
        where extract(year from h.date) = year
          and extract(dow from h.date) between 1 and 5
        group by period
    )
    select 
        w.period,
        w.working_days,
        coalesce(h.holidays, 0) as holidays,
        (w.working_days * interval '8 hours') as hours8,
        (w.working_days * interval '8 hours 30 minutes') as hours85
    from workdays w
    left join holidays h on w.period = h.period
    on conflict (period) do update
        set working_days = excluded.working_days,
            holidays = excluded.holidays,
            hours8 = excluded.hours8,
            hours85 = excluded.hours85;
end;
$$ language plpgsql;


select attendance.generate_work_calendar(2025);



commit;
