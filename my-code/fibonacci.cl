class BinaryNode {
    io: IO;
    m_data: String;
    m_left: BinaryNode;
    m_right: BinaryNode;

    init(data: String, left: BinaryNode, right: BinaryNode): SELF_TYPE {
        {
            m_data <- data;
            m_left <- left;
            m_right <- right;
            self;
        }
    };

    print(): String{
        if isvoid m_left then
            if isvoid m_right then
                m_data
            else
                m_data.concat(m_right.print())
            fi
        else
            if isvoid m_right then
                m_data.concat(m_left.print())
            else
                m_data.concat(m_left.print().concat(m_right.print()))
            fi
        fi
    };
};

class Main inherits A2I {
    io: IO;
    nil: BinaryNode;
    fibonacci(x: Int) : Int{
        if x <= 1 then
            x
        else
            fibonacci(x - 1) + fibonacci(x - 2)
        fi
    };

    main(): Int{
        {
            io <- new IO;
            let x: BinaryNode <- (new BinaryNode).init("hello", (new BinaryNode).init("world", nil, nil), (new BinaryNode).init("!", nil, nil)) in {
                io.out_string(x.print());
            };
            0;
        }
    };
};