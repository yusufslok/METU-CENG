def inheritance(lis):
    departed = []
    for i in lis:
        if i[:8] ==  "DEPARTED":
            u = i.split()
            for j in range(len(u)):
                u[j] = u[j].strip()
            departed += [u[1]]
    for i in lis:
        if i[:8] == "DECEASED":
            dec_item = i
    inputs = dec_item.split()
    for d in range(len(inputs)):
        inputs[d] = inputs[d].strip()
    deceasedperson = inputs[1]
    heritage = eval(inputs[2])
    def alive_spouse(person,lis):
        for i in lis:
            if i[:7] == "MARRIED" and (person in i):
                b = i.split()
                for d in range(len(b)):
                    b[d] = b[d].strip()
                if person == b[1]:
                    spouse = b[2]
                elif person == b[2]:
                    spouse = b[1]
        if have_descendant(spouse,lis):
            return spouse
    def ismarriedandalive(person,lis):
        spouse = []
        spouse_number = 0
        for i in lis:
            if i[:7] == "MARRIED" and (person in i):
                b = i.split()
                for d in range(len(b)):
                    b[d] = b[d].strip()
                if person == b[1]:
                    spouse += [b[2]]
                elif person == b[2]:
                    spouse += [b[1]]
        for i in spouse:
            if have_descendant(i,lis):
                spouse_number += 1
        if spouse_number == 0:
            return False
        else:
            return True
    def have_descendant(person,lis):
        nonlocal departed
        if not ( person in departed ) :
            return True
        else:
            return False
    def have_descendant_helper(person,lis):
        for i in person:
            for j in lis:
                b = j.split()
                for d in range(len(b)):
                    b[d] = b[d].strip()
                if b[0] == "CHILD" and (b[1] == i or b[2] == i):
                    for h in b[3:]:
                        print(person)
                        person += [h]
        for i in person:
            if have_descendant(i,lis):
                return True
        return False
    def descendant_division(person,lis):
        division_number = 0
        for j in lis:
            b = j.split()
            for d in range(len(b)):
                b[d] = b[d].strip()
            if b[0] == "CHILD" and (b[1] == person or b[2] == person):
                for h in b[3:]:
                    if have_descendant_helper([h],lis):
                        division_number += 1
        return division_number
    result= []
    def child_amount(person, lis, amount):
        nonlocal result
        for j in lis:
            b = j.split()
            for d in range(len(b)):
                b[d] = b[d].strip()
            if b[0] == "CHILD" and (b[1] == person or b[2] == person):
                for h in b[3:]:
                    if not (h in departed):
                        result += [(h, amount/descendant_division(person,lis))]
                    elif have_descendant_helper([h],lis):
                        child_amount_helper(h, lis, amount/descendant_division(person,lis))
        return result
    def child_amount_helper(person, lis, amount):
        nonlocal result
        for j in lis:
            b = j.split()
            for d in range(len(b)):
                b[d] = b[d].strip()
            if b[0] == "CHILD" and (b[1] == person or b[2] == person):
                for h in b[3:]:
                    if not (h in departed):
                        result += [(h, amount/descendant_division(person,lis))]
                    elif have_descendant_helper([h], lis):
                        child_amount(h, lis, amount/descendant_division(person,lis))
        return result
    def have_parents(deceasedperson,lis):
        for j in lis:
            b = j.split()
            for d in range(len(b)):
                b[d] = b[d].strip()
            if b[0] == "CHILD" and (deceasedperson in b[3:]) and (have_descendant_helper([b[1]],lis) or have_descendant_helper([b[2]],lis)):
                return True
        return False
    def parent_amount(deceasedperson,lis):
        number = 0
        for j in lis:
            b = j.split()
            for d in range(len(b)):
                b[d] = b[d].strip()
            if b[0] == "CHILD" and (deceasedperson in b[3:]):
                if have_descendant_helper([b[1]],lis):
                    number += 1
                if have_descendant_helper([b[2]],lis):
                    number += 1
        return number
    def number_of_grandparents_have_descendant(deceasedperson,lis):
        number = 0
        for j in lis:
            b = j.split()
            for d in range(len(b)):
                b[d] = b[d].strip()
            if b[0] == "CHILD" and (deceasedperson in b[3:]):
                for i in [b[1],b[2]]:
                    for h in lis:
                        g = h.split()
                        for s in range(len(g)):
                            g[s] = g[s].strip()
                        if g[0] == "CHILD" and (i in g[3:]):
                            if have_descendant_helper([g[1]],lis):
                                number += 1
                            if have_descendant_helper([g[2]],lis):
                                number += 1
        return number
    if ismarriedandalive(deceasedperson,lis) and descendant_division(deceasedperson,lis):
        result += [(alive_spouse(deceasedperson,lis),heritage / 4)]
        child_amount(deceasedperson,lis,heritage * 3 / 4 )
    elif not ismarriedandalive(deceasedperson,lis) and descendant_division(deceasedperson,lis):
        child_amount(deceasedperson, lis, heritage)
    elif ismarriedandalive(deceasedperson,lis) and have_parents(deceasedperson,lis):
        result += [(alive_spouse(deceasedperson,lis),heritage / 2)]
        for j in lis:
            b = j.split()
            for d in range(len(b)):
                b[d] = b[d].strip()
            if b[0] == "CHILD" and (deceasedperson in b[3:]):
                if have_descendant(b[1],lis):
                    result += [(b[1], (heritage / 2) / parent_amount(deceasedperson, lis))]
                elif have_descendant_helper([b[1]],lis):
                    child_amount(b[1], lis, (heritage / 2) / parent_amount(deceasedperson, lis))
                if have_descendant(b[2],lis):
                    result += [(b[2], (heritage / 2) / parent_amount(deceasedperson, lis))]
                elif have_descendant_helper([b[1]],lis):
                    child_amount(b[2], lis, (heritage / 2) / parent_amount(deceasedperson, lis))
    elif not ismarriedandalive(deceasedperson,lis) and have_parents(deceasedperson,lis):
        for j in lis:
            b = j.split()
            for d in range(len(b)):
                b[d] = b[d].strip()
            if b[0] == "CHILD" and (deceasedperson in b[3:]):
                if have_descendant(b[1],lis):
                    result += [(b[1], (heritage) / parent_amount(deceasedperson, lis))]
                elif have_descendant_helper([b[1]],lis):
                    child_amount(b[1], lis, (heritage) / parent_amount(deceasedperson, lis))
                if have_descendant(b[2],lis):
                    result += [(b[2], (heritage) / parent_amount(deceasedperson, lis))]
                elif have_descendant_helper([b[1]],lis):
                    child_amount(b[2], lis, (heritage) / parent_amount(deceasedperson, lis))
    elif ismarriedandalive(deceasedperson,lis) and number_of_grandparents_have_descendant(deceasedperson,lis):
        result += [(alive_spouse(deceasedperson,lis), heritage * 3 / 4)]
        for j in lis:
            b = j.split()
            for d in range(len(b)):
                b[d] = b[d].strip()
            if b[0] == "CHILD" and (deceasedperson in b[3:]):
                for i in [b[1], b[2]]:
                    for h in lis:
                        g = h.split()
                        for s in range(len(g)):
                            g[s] = g[s].strip()
                        if g[0] == "CHILD" and (i in g[3:]):
                            if have_descendant(g[1],lis):
                                result += [(g[1], heritage / 4 / number_of_grandparents_have_descendant(deceasedperson,lis))]
                            else:
                                child_amount(g[1], lis, heritage / 4 / number_of_grandparents_have_descendant(deceasedperson,lis))
                            if have_descendant(g[2],lis):
                                result += [(g[2], heritage / 4 / number_of_grandparents_have_descendant(deceasedperson,lis))]
                            else:
                                child_amount(g[2], lis, heritage / 4 / number_of_grandparents_have_descendant(deceasedperson,lis))
    elif not ismarriedandalive(deceasedperson, lis) and number_of_grandparents_have_descendant(deceasedperson, lis):
        for j in lis:
            b = j.split()
            for d in range(len(b)):
                b[d] = b[d].strip()
            if b[0] == "CHILD" and (deceasedperson in b[3:]):
                for i in [b[1], b[2]]:
                    for h in lis:
                        g = h.split()
                        for s in range(len(g)):
                            g[s] = g[s].strip()
                        if g[0] == "CHILD" and (i in g[3:]):
                            if have_descendant(g[1],lis):
                                result += [(g[1], heritage / number_of_grandparents_have_descendant(deceasedperson,lis))]
                            else:
                                child_amount(g[1], lis, heritage / number_of_grandparents_have_descendant(deceasedperson,lis))
                            if have_descendant(g[2],lis):
                                result += [(g[2], heritage / number_of_grandparents_have_descendant(deceasedperson,lis))]
                            else:
                                child_amount(g[2], lis, heritage / number_of_grandparents_have_descendant(deceasedperson,lis))
    elif ismarriedandalive(deceasedperson, lis) and not number_of_grandparents_have_descendant(deceasedperson, lis):
        result += [(alive_spouse(deceasedperson,lis),heritage)]
    for i in range(len(result)):
        for j in range(i + 1, len(result)):
            if i < len(result) and j < len(result):
                if result[i][0] == result[j][0]:
                    b = result[i][0]
                    c = result[i][1] + result[j][1]
                    result.pop(j)
                    result.pop(i)
                    result.insert(i, (b, c))
    return result
print(inheritance(["CHILD J SJ A B C", "CHILD SJ H D", "MARRIED SJ H", "DEPARTED SJ", "DEPARTED A", "DECEASED A 600"]))
