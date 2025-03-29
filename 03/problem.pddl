(define (problem mapf)
    (:domain graph-with-agents)

    (:objects
        rur, lul - agent
        c, d, f, b, a, e - vertex
    )

    (:init
        (is-at rur a)
        (occupied a)        (is-at lul d)
        (occupied d)
        (edge b c)        (edge c b)        (edge a b)        (edge b a)        (edge e d)        (edge d e)        (edge b d)        (edge d b)        (edge d c)        (edge c d)        (edge e f)        (edge f e)    )

    (:goal ( and
        (is-at rur f)
        (is-at lul b)
    ))
)
