#include <CHDManProgressThread.h>

CHDManProgressThread::CHDManProgressThread(wxProcess *process, wxEvtHandler *threadEventHandler, wxMessageQueue<ThreadMessage> *messageQueue) : wxThread(wxTHREAD_JOINABLE)
{
    m_messageQueue = messageQueue;
    m_threadEventHandler = threadEventHandler;
    m_readBuffer = new char[1024];
    m_process = process;
}

CHDManProgressThread::~CHDManProgressThread()
{
    delete[] m_readBuffer;
}

wxThread::ExitCode CHDManProgressThread::Entry()
{
    wxThread::ExitCode exitCode;
    ThreadMessage m = MessageLast;

    while (true)
    {
        if (TestDestroy())
        {
            wxProcess::Kill(m_process->GetPid());
            break;
        }

        m = MessageLast;
        wxMessageQueueError messageQueueError = m_messageQueue->ReceiveTimeout(10, m);

        if (messageQueueError == wxMSGQUEUE_NO_ERROR)
        {
            if (m == ThreadMessage::ProcessComplete)
            {
                break;
            }
            else if (m == ThreadMessage::ExitThread)
            {
                return (wxThread::ExitCode)-1;
            }
        }

        while (m_process->IsInputOpened())
        {
            auto processOutputStream = m_process->GetInputStream();
            auto processErrorStream = m_process->GetErrorStream();
            wxTextInputStream errorStreamReader(*processErrorStream);

            while (m_process->IsInputAvailable())
            {
                processOutputStream->Read(m_readBuffer, 1024);
                auto threadEvent = new wxThreadEvent(wxEVT_THREAD_STDIN);
                threadEvent->SetString(wxString(m_readBuffer, processOutputStream->LastRead()));

                m_threadEventHandler->QueueEvent(threadEvent);
            }

            while (m_process->IsErrorAvailable())
            {
                auto errorLine = errorStreamReader.ReadLine();
                auto threadEvent = new wxThreadEvent(wxEVT_THREAD_STDERR);
                auto progressEvent = new CHDManProgressEvent();
                progressEvent->processId = m_process->GetPid();
                progressEvent->message = errorLine;
                threadEvent->SetString(errorLine);
                threadEvent->SetPayload<CHDManProgressEvent*>(progressEvent);
                m_threadEventHandler->QueueEvent(threadEvent);
            }
        }
    }

    return exitCode;
}