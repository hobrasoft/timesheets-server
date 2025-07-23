begin;

insert into version (version) values (14);

drop view if exists attendance.present cascade;

create view attendance.present as (
select emp.*, x.date, x.event_type,
       case when x.arrival then true
       else false
            end as present
    from attendance.employees emp
    left join lateral (select e.date, et.*
        from attendance.events e,
             attendance.event_types et
        where e.employee = emp.employee
          and e.event_type = et.event_type
          and e.date < now()
          and e.date > now() - '2days'::interval
        order by date desc
        limit 1) x on true
    where emp.active
);

commit;

