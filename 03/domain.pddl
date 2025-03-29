(define (domain graph-with-agents)
        (:requirements :strips :typing :adl)
        (:types vertex agent)
        (:predicates
              (occupied ?v - vertex)
              (is-at ?a - agent ?v - vertex)
              (edge ?a ?b - vertex)
        )

        (:action move
                :parameters (?a - agent ?s ?d - vertex)
                :precondition (and
                        (is-at ?a ?s)
                        (occupied ?s)
                        (not (occupied ?d))
                        (or
                                (edge ?d ?s)
                                (edge ?s ?d)
                        )
                )
                :effect (and
                        (not (is-at ?a ?s))
                        (is-at ?a ?d)
                        (not (occupied ?s))
                        (occupied ?d)
                )
        )
)

