begin;

insert into version (version) values (7);

drop view if exists attendance.events_view;

create view attendance.events_view as

select
        e.event,
        e.date,
        e.event_type,
        t.description as event_description,
        e.employee,
        p.firstname,
        p.surname,
        e.valid,
        e.user_edited,
        u.name as user_edited_name,
        case when e.valid and     t.end_state and     prev.end_state then 'Unexpected END'  else 
        case when e.valid and not t.end_state and not prev.end_state then 'Missing END'     else 
            null
            end end as error

    from attendance.events e
    left join attendance.employees   p using (employee)
    left join users                  u on (e.user_edited = u."user")
    left join attendance.event_types t using (event_type)

    left join lateral (select * 
            from attendance.events ep
            left join attendance.event_types t using (event_type)
            where ep.valid
              and ep.date < e.date
              and ep.employee = e.employee
              and not t.passage
            order by date desc
            limit 1
            ) prev on true

    left join lateral (select * 
            from attendance.events ep
            left join attendance.event_types t using (event_type)
            where ep.valid
              and ep.date > e.date
              and ep.employee = e.employee
              and not t.passage
            order by date 
            limit 1
            ) next on true

    ;

commit;
