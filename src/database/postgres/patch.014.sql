begin;

insert into version (version) values (14);

drop view if exists attendance.present cascade;

create view attendance.present as (

select emp.*, 
       case when x.arrival then x.date else k.date end as date, 
       case when x.arrival then x.event_type else k.event_type end as event_type,
       case when x.arrival then true else false end as present
    from attendance.employees emp
    left join lateral (select e.date, et.*
        from attendance.events e,
             attendance.event_types et
        where e.employee = emp.employee
          and e.event_type = et.event_type
          and e.date < now()
          and e.date > now() - '2days'::interval
          and e.valid
        order by date desc
        limit 1) x on true

    left join lateral (select e.date, et.*
        from attendance.events e,
             attendance.event_types et
        where e.employee = emp.employee
          and e.event_type = et.event_type
          and e.date <= now() - '2days'::interval
          and et.arrival
          and e.valid
        order by date desc
        limit 1) a on true

    left join lateral (select e.date, et.*
        from attendance.events e,
             attendance.event_types et
        where e.employee = emp.employee
          and e.event_type = et.event_type
          and et.end_state
          and e.date > a.date
          and e.valid
        order by date 
        limit 1) k on true

    where emp.active

);

commit;

